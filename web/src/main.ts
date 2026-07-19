import "./style.css";
import {
  defaultProfile,
  fetchHome,
  fetchProjectDetail,
  type PortfolioProject,
  type ProjectListData,
  type SiteProfile
} from "./runtime/api";
import {
  renderCatalogPage,
  renderErrorState,
  renderLoadingState,
  renderProjectDetail,
  renderShell
} from "./runtime/htmlRenderer";
import {
  buildCatalogUrl,
  buildProjectUrl,
  getCurrentAppLocation,
  type AppLocation
} from "./runtime/location";

type AppState = {
  currentLocation: AppLocation;
  projectList: ProjectListData | null;
  projectDetail: PortfolioProject | null;
  profile: SiteProfile;
  loading: boolean;
  error: string;
  selectedFilter: string;
  searchTerm: string;
};

const root = document.querySelector<HTMLDivElement>("#app");

if (!root) {
  throw new Error("Missing #app root element");
}

const appRoot = root;

const state: AppState = {
  currentLocation: getCurrentAppLocation(),
  projectList: null,
  projectDetail: null,
  profile: defaultProfile,
  loading: true,
  error: "",
  selectedFilter: "All",
  searchTerm: ""
};

function visibleProjects(data: ProjectListData): PortfolioProject[] {
  const query = state.searchTerm.trim().toLowerCase();
  return data.items.filter((project) => {
    const matchesFilter =
      state.selectedFilter === "All" || project.category === state.selectedFilter;
    const matchesQuery =
      !query ||
      project.name.toLowerCase().includes(query) ||
      project.description.toLowerCase().includes(query) ||
      project.techStack.some((item) => item.toLowerCase().includes(query));
    return matchesFilter && matchesQuery;
  });
}

function pageTitle(): string {
  if (state.currentLocation.page === "project" && state.projectDetail) {
    return `${state.projectDetail.name} | ${state.profile.siteName}`;
  }
  return `${state.profile.siteName} | ${state.profile.subtitle}`;
}

function render(): void {
  let content = "";

  if (state.loading) {
    content = renderLoadingState();
  } else if (state.error) {
    content = renderErrorState(state.error);
  } else if (state.currentLocation.page === "catalog" && state.projectList) {
    content = renderCatalogPage(
      state.projectList,
      visibleProjects(state.projectList),
      state.selectedFilter,
      state.searchTerm
    );
  } else if (state.currentLocation.page === "project" && state.projectDetail) {
    content = renderProjectDetail(state.projectDetail, state.profile);
  } else {
    content = renderErrorState("页面状态异常，请返回项目列表后重试。");
  }

  appRoot.innerHTML = renderShell(content, state.profile);
  document.title = pageTitle();
}

async function loadCatalogPage(): Promise<void> {
  state.loading = true;
  state.error = "";
  state.projectDetail = null;
  render();

  const data = await fetchHome();
  state.projectList = data;
  state.profile = data.profile;
  state.loading = false;
  render();
}

async function loadProjectPage(slug: string): Promise<void> {
  state.loading = true;
  state.error = "";
  state.projectDetail = null;
  render();

  const data = await fetchProjectDetail(slug);
  state.profile = state.projectList?.profile ?? defaultProfile;
  state.projectDetail = data.project;
  state.loading = false;
  render();
}

function syncFromLocation(): void {
  state.currentLocation = getCurrentAppLocation();
}

async function loadCurrentPage(): Promise<void> {
  if (state.currentLocation.page === "catalog") {
    await loadCatalogPage();
    return;
  }

  await loadProjectPage(state.currentLocation.slug);
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

function handleAction(element: HTMLElement, event: Event): boolean {
  const action = element.dataset.action;
  if (!action) {
    return false;
  }

  switch (action) {
    case "home":
      event.preventDefault();
      void navigate(buildCatalogUrl());
      return true;
    case "open-project": {
      event.preventDefault();
      const slug = element.dataset.slug;
      if (slug) {
        void navigate(buildProjectUrl(slug));
      }
      return true;
    }
    case "set-filter": {
      event.preventDefault();
      state.selectedFilter = element.dataset.filter || "All";
      render();
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
  if (url.origin !== window.location.origin || url.pathname.startsWith("/assets/")) {
    return;
  }

  if (url.pathname.includes("/pages/")) {
    return;
  }

  if (url.pathname === window.location.pathname && url.hash) {
    event.preventDefault();
    if (`${url.pathname}${url.hash}` !== `${window.location.pathname}${window.location.hash}`) {
      window.history.pushState({}, "", `${url.pathname}${url.hash}`);
    }
    scrollToHash();
    return;
  }

  event.preventDefault();
  void navigate(`${url.pathname}${url.hash}`);
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

appRoot.addEventListener("input", (event) => {
  const target = event.target;
  if (!(target instanceof HTMLInputElement) || target.dataset.action !== "search-projects") {
    return;
  }

  state.searchTerm = target.value;
  render();
  const nextInput = appRoot.querySelector<HTMLInputElement>('[data-action="search-projects"]');
  nextInput?.focus();
});

window.addEventListener("popstate", () => {
  syncFromLocation();
  void loadCurrentPage();
});

render();
void loadCurrentPage()
  .then(scrollToHash)
  .catch((error: unknown) => {
    state.loading = false;
    state.error = error instanceof Error ? error.message : String(error);
    render();
  });
