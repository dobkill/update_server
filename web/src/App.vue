<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref, watchEffect } from "vue";
import ProductCatalogPage from "./pages/ProductCatalogPage.vue";
import ReleaseDetailPage from "./pages/ReleaseDetailPage.vue";
import { composeReleaseBlocks } from "./runtime/pageComposer";
import {
  defaultReleaseChannel,
  fetchPortfolioHome,
  fetchReleaseDetail,
  type PortfolioHomeData,
  type ProductReleaseDetail,
  type SiteProfile
} from "./runtime/api";
import { getMockPortfolioHome, getMockReleaseDetail } from "./runtime/mockData";
import {
  buildCatalogUrl,
  buildReleaseUrl,
  getCurrentAppLocation,
  latestVersionAlias
} from "./runtime/location";
import AppShell from "./shell/AppShell.vue";

const currentLocation = ref(getCurrentAppLocation());
const portfolioHome = ref<PortfolioHomeData>(getMockPortfolioHome());
const release = ref<ProductReleaseDetail | null>(null);
const loading = ref(true);
const error = ref("");

const emptyProfile: SiteProfile = {
  site_name: "YXX Works",
  subtitle: "产品、插件与创作实验",
  github_url: "",
  email: ""
};

const title = computed(() => {
  if (currentLocation.value.page === "catalog") {
    return `${portfolioHome.value.profile.site_name || "YXX Works"} | ${portfolioHome.value.profile.subtitle || "产品、插件与创作实验"}`;
  }

  return release.value
    ? `${release.value.title} | ${release.value.product_code}`
    : "版本详情 | 更新平台";
});

const releaseBlocks = computed(() => (release.value ? composeReleaseBlocks(release.value) : []));
const shellProfile = computed(() => portfolioHome.value?.profile ?? emptyProfile);
const currentProductCode = computed(() =>
  currentLocation.value.page === "release" ? currentLocation.value.productCode : ""
);
const currentVersion = computed(() =>
  currentLocation.value.page === "release" && release.value
    ? release.value.resolved_version
    : currentLocation.value.page === "release"
      ? currentLocation.value.version
      : ""
);
const currentChannel = computed(() =>
  currentLocation.value.page === "release" ? currentLocation.value.channel : defaultReleaseChannel
);

async function loadCatalogPage() {
  loading.value = true;
  error.value = "";
  release.value = null;

  const remoteHome = await fetchPortfolioHome();
  if (remoteHome) {
    portfolioHome.value = remoteHome;
    loading.value = false;
    return;
  }

  portfolioHome.value = getMockPortfolioHome();
  error.value = "";
  loading.value = false;
}

async function loadReleasePage(productCode: string, version: string, channel: string) {
  loading.value = true;
  error.value = "";

  const remoteRelease = await fetchReleaseDetail(productCode, version, channel);
  if (remoteRelease) {
    release.value = remoteRelease;
    loading.value = false;
    return;
  }

  const fallbackRelease = getMockReleaseDetail(productCode, version, channel);
  if (fallbackRelease) {
    release.value = fallbackRelease;
    error.value = `接口暂不可用，当前显示 ${channel} 渠道的本地示例数据。`;
  } else {
    release.value = null;
    error.value = `未找到 ${productCode} ${version} 的 ${channel} 渠道数据。`;
  }

  loading.value = false;
}

function syncFromLocation() {
  currentLocation.value = getCurrentAppLocation();
}

async function loadCurrentPage() {
  if (currentLocation.value.page === "catalog") {
    await loadCatalogPage();
    return;
  }

  await loadReleasePage(
    currentLocation.value.productCode,
    currentLocation.value.version,
    currentLocation.value.channel
  );
}

function handlePopState() {
  syncFromLocation();
  void loadCurrentPage();
}

function navigate(targetUrl: string) {
  if (targetUrl === `${window.location.pathname}${window.location.search}`) {
    return;
  }

  window.history.pushState({}, "", targetUrl);
  syncFromLocation();
  void loadCurrentPage();
}

function openCatalog() {
  navigate(buildCatalogUrl());
}

function openProduct(productCode: string) {
  navigate(buildReleaseUrl(productCode, latestVersionAlias, currentChannel.value));
}

onMounted(async () => {
  window.addEventListener("popstate", handlePopState);
  syncFromLocation();
  await loadCurrentPage();
});

onUnmounted(() => {
  window.removeEventListener("popstate", handlePopState);
});

watchEffect(() => {
  document.title = title.value;
});
</script>

<template>
  <AppShell
    :current-product-code="currentProductCode"
    :current-version="currentVersion"
    :profile="shellProfile"
    @home="openCatalog"
  >
    <section v-if="loading" class="state-card">
      <strong>加载中</strong>
      <span>正在读取发布数据...</span>
    </section>

    <section v-else-if="error" class="state-card warning">
      <strong>提示</strong>
      <span>{{ error }}</span>
    </section>

    <ProductCatalogPage
      v-if="!loading && currentLocation.page === 'catalog'"
      :home="portfolioHome"
      @open="openProduct"
    />

    <ReleaseDetailPage
      v-else-if="!loading && release && currentLocation.page === 'release'"
      :release="release"
      :blocks="releaseBlocks"
      @back="openCatalog"
    />
  </AppShell>
</template>

<style scoped>
.state-card {
  display: flex;
  gap: 0.65rem;
  align-items: center;
  margin: 1rem auto 0.4rem;
  padding: 0.85rem 1rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: rgba(255, 255, 255, 0.86);
  box-shadow: var(--shadow-soft);
}

.state-card.warning {
  border-color: rgba(183, 121, 31, 0.35);
  background: #fff8ec;
}

.state-card span {
  color: var(--muted);
}

@media (max-width: 560px) {
  .state-card {
    flex-direction: column;
    align-items: flex-start;
  }
}
</style>
