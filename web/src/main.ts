import "./style.css";
import { defaultReleaseChannel, fetchPortfolioHome, fetchReleaseDetail } from "./runtime/api";
import type { PortfolioHomeData, ProductReleaseDetail, SiteProfile } from "./runtime/api";
import {
  renderCatalogPage,
  renderErrorState,
  renderLoadingState,
  renderReleasePage,
  renderShell,
  renderSoftErrorBar
} from "./runtime/htmlRenderer";
import { getMockPortfolioHome, getMockReleaseDetail } from "./runtime/mockData";
import { composeReleaseBlocks } from "./runtime/pageComposer";
import {
  buildCatalogUrl,
  buildReleaseUrl,
  getCurrentAppLocation,
  latestVersionAlias
} from "./runtime/location";
import type { AppLocation } from "./runtime/location";

type AppState = {
  currentLocation: AppLocation;
  portfolioHome: PortfolioHomeData;
  release: ProductReleaseDetail | null;
  loading: boolean;
  error: string;
  mobileMenuOpen: boolean;
};

const root = document.querySelector<HTMLDivElement>("#app");

if (!root) {
  throw new Error("Missing #app root element");
}

const appRoot = root;

const emptyProfile: SiteProfile = {
  site_name: "YXX Works",
  subtitle: "产品、插件与创作实验",
  github_url: "",
  email: ""
};

const state: AppState = {
  currentLocation: getCurrentAppLocation(),
  portfolioHome: getMockPortfolioHome(),
  release: null,
  loading: true,
  error: "",
  mobileMenuOpen: false
};

function getTitle(): string {
  if (state.currentLocation.page === "catalog") {
    const profile = state.portfolioHome.profile;
    return `${profile.site_name || "YXX Works"} | ${profile.subtitle || "产品、插件与创作实验"}`;
  }

  return state.release
    ? `${state.release.title} | ${state.release.product_code}`
    : "版本详情 | 更新平台";
}

function getShellProfile(): SiteProfile {
  return state.portfolioHome?.profile ?? emptyProfile;
}

function getCurrentProductCode(): string {
  return state.currentLocation.page === "release" ? state.currentLocation.productCode : "";
}

function getCurrentVersion(): string {
  if (state.currentLocation.page !== "release") {
    return "";
  }
  return state.release ? state.release.resolved_version : state.currentLocation.version;
}

function getCurrentChannel(): string {
  return state.currentLocation.page === "release" ? state.currentLocation.channel : defaultReleaseChannel;
}

function render(): void {
  let content = "";

  if (state.loading) {
    content = renderLoadingState();
  } else if (state.error && !state.release) {
    content = renderErrorState(state.error);
  } else if (state.currentLocation.page === "catalog") {
    content = renderCatalogPage(state.portfolioHome);
  } else if (state.release) {
    content = renderReleasePage(state.release, composeReleaseBlocks(state.release));
  } else {
    content = renderErrorState("页面状态异常，请返回产品列表后重试。");
  }

  appRoot.innerHTML =
    renderShell(content, {
      currentProductCode: getCurrentProductCode(),
      currentVersion: getCurrentVersion(),
      mobileMenuOpen: state.mobileMenuOpen,
      profile: getShellProfile()
    }) + (state.error && state.release ? renderSoftErrorBar(state.error) : "");

  document.title = getTitle();
}

async function loadCatalogPage(): Promise<void> {
  state.loading = true;
  state.error = "";
  state.release = null;
  render();

  const remoteHome = await fetchPortfolioHome();
  state.portfolioHome = remoteHome ?? getMockPortfolioHome();
  state.loading = false;
  render();
}

async function loadReleasePage(productCode: string, version: string, channel: string): Promise<void> {
  state.loading = true;
  state.error = "";
  render();

  const remoteRelease = await fetchReleaseDetail(productCode, version, channel);
  if (remoteRelease) {
    state.release = remoteRelease;
    state.loading = false;
    render();
    return;
  }

  const fallbackRelease = getMockReleaseDetail(productCode, version, channel);
  if (fallbackRelease) {
    state.release = fallbackRelease;
    state.error = `接口暂不可用，当前显示 ${channel} 渠道的本地示例数据。`;
  } else {
    state.release = null;
    state.error = `未找到 ${productCode} ${version} 的 ${channel} 渠道数据。`;
  }

  state.loading = false;
  render();
}

function syncFromLocation(): void {
  state.currentLocation = getCurrentAppLocation();
  state.mobileMenuOpen = false;
}

async function loadCurrentPage(): Promise<void> {
  if (state.currentLocation.page === "catalog") {
    await loadCatalogPage();
    return;
  }

  await loadReleasePage(
    state.currentLocation.productCode,
    state.currentLocation.version,
    state.currentLocation.channel
  );
}

function scrollToHash(): void {
  const hash = window.location.hash.slice(1);
  if (!hash) {
    window.scrollTo({ top: 0 });
    return;
  }

  window.setTimeout(() => {
    document.getElementById(decodeURIComponent(hash))?.scrollIntoView({
      behavior: "smooth",
      block: "start"
    });
  }, 0);
}

async function navigate(targetUrl: string): Promise<void> {
  const currentUrl = `${window.location.pathname}${window.location.search}${window.location.hash}`;
  if (targetUrl === currentUrl) {
    return;
  }

  window.history.pushState({}, "", targetUrl);
  syncFromLocation();
  await loadCurrentPage();
  scrollToHash();
}

function openCatalog(): void {
  void navigate(buildCatalogUrl());
}

function openProduct(productCode: string): void {
  void navigate(buildReleaseUrl(productCode, latestVersionAlias, getCurrentChannel()));
}

function handleAction(element: HTMLElement, event: MouseEvent): boolean {
  const action = element.dataset.action;
  if (!action) {
    return false;
  }

  switch (action) {
    case "home":
    case "back":
      event.preventDefault();
      openCatalog();
      return true;
    case "toggle-menu":
      event.preventDefault();
      state.mobileMenuOpen = !state.mobileMenuOpen;
      render();
      return true;
    case "close-menu":
      state.mobileMenuOpen = false;
      return false;
    case "open-product": {
      event.preventDefault();
      const productCode = element.dataset.productCode;
      if (productCode) {
        openProduct(productCode);
      }
      return true;
    }
    default:
      return false;
  }
}

function handleInternalLink(anchor: HTMLAnchorElement, event: MouseEvent): void {
  if (anchor.target || anchor.hasAttribute("download")) {
    return;
  }

  const href = anchor.getAttribute("href");
  if (!href || href.startsWith("mailto:") || href.startsWith("tel:")) {
    return;
  }

  const url = new URL(anchor.href);
  if (url.origin !== window.location.origin || url.pathname.startsWith("/data/") || url.pathname.startsWith("/api/")) {
    return;
  }

  if (url.pathname === window.location.pathname && url.search === window.location.search && url.hash) {
    event.preventDefault();
    state.mobileMenuOpen = false;
    if (`${url.pathname}${url.search}${url.hash}` !== `${window.location.pathname}${window.location.search}${window.location.hash}`) {
      window.history.pushState({}, "", `${url.pathname}${url.search}${url.hash}`);
    }
    render();
    scrollToHash();
    return;
  }

  event.preventDefault();
  void navigate(`${url.pathname}${url.search}${url.hash}`);
}

appRoot.addEventListener("click", (event) => {
  const target = event.target;
  if (!(target instanceof Element)) {
    return;
  }

  const actionElement = target.closest<HTMLElement>("[data-action]");
  if (actionElement && appRoot.contains(actionElement) && handleAction(actionElement, event)) {
    return;
  }

  const anchor = target.closest<HTMLAnchorElement>("a[href]");
  if (anchor && appRoot.contains(anchor)) {
    handleInternalLink(anchor, event);
  }
});

window.addEventListener("popstate", () => {
  syncFromLocation();
  void loadCurrentPage();
});

render();
void loadCurrentPage().catch((error: unknown) => {
  state.loading = false;
  state.error = `页面加载失败：${error instanceof Error ? error.message : String(error)}`;
  render();
});
