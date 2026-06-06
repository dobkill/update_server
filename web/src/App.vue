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
    <Transition name="page-fade" mode="out-in">
      <section v-if="loading" key="loading" class="state-card loading-state">
        <div class="spinner" aria-hidden="true">
          <span></span>
        </div>
        <div class="state-copy">
          <strong>加载中</strong>
          <span>正在读取发布数据…</span>
        </div>
      </section>

      <section v-else-if="error && !release" key="error" class="state-card error-state">
        <div class="error-icon" aria-hidden="true">⚠</div>
        <div class="state-copy">
          <strong>提示</strong>
          <span>{{ error }}</span>
        </div>
      </section>

      <ProductCatalogPage
        v-else-if="currentLocation.page === 'catalog'"
        key="catalog"
        :home="portfolioHome"
        @open="openProduct"
      />

      <ReleaseDetailPage
        v-else-if="release && currentLocation.page === 'release'"
        key="release"
        :release="release"
        :blocks="releaseBlocks"
        @back="openCatalog"
      />
    </Transition>

    <!-- Soft error bar when data loaded from fallback -->
    <Transition name="slide-up">
      <div v-if="error && release" class="soft-error-bar">
        <span>{{ error }}</span>
      </div>
    </Transition>
  </AppShell>
</template>

<style scoped>
/* ─── Loading State ─── */
.loading-state {
  display: flex;
  gap: var(--space-m);
  align-items: center;
  justify-content: center;
  margin: var(--space-xl) auto;
  padding: var(--space-l) var(--space-xl);
  border: 1px solid var(--line);
  border-radius: var(--radius-lg);
  background: rgba(255, 255, 255, 0.9);
  box-shadow: var(--shadow-soft);
}

.spinner {
  position: relative;
  width: 2rem;
  height: 2rem;
  flex-shrink: 0;
}

.spinner span {
  position: absolute;
  inset: 0;
  border: 2.5px solid var(--line);
  border-top-color: var(--primary);
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}

/* ─── Error State ─── */
.error-state {
  display: flex;
  gap: var(--space-m);
  align-items: flex-start;
  margin: var(--space-xl) auto;
  padding: var(--space-l) var(--space-xl);
  border: 1px solid rgba(220, 80, 60, 0.25);
  border-radius: var(--radius-lg);
  background: #fff6f5;
  box-shadow: var(--shadow-soft);
}

.error-icon {
  font-size: 1.4rem;
  line-height: 1;
  flex-shrink: 0;
}

/* ─── Shared state card ─── */
.state-card .state-copy {
  display: grid;
  gap: 0.2rem;
  min-width: 0;
}

.state-card .state-copy span {
  color: var(--muted);
  font-size: var(--font-sm);
}

/* ─── Soft Error Bar ─── */
.soft-error-bar {
  position: fixed;
  bottom: 1rem;
  left: 50%;
  transform: translateX(-50%);
  z-index: 200;
  padding: 0.55rem 1.1rem;
  border: 1px solid rgba(183, 121, 31, 0.3);
  border-radius: var(--radius-xl);
  background: #fff8ec;
  color: #8a6d2b;
  font-size: var(--font-sm);
  box-shadow: var(--shadow-soft);
  white-space: nowrap;
  max-width: 90vw;
  overflow: hidden;
  text-overflow: ellipsis;
}

/* ─── Page Fade Transition ─── */
.page-fade-enter-active,
.page-fade-leave-active {
  transition: opacity var(--transition-normal);
}

.page-fade-enter-from,
.page-fade-leave-to {
  opacity: 0;
}

/* ─── Slide-up Transition ─── */
.slide-up-enter-active,
.slide-up-leave-active {
  transition: all var(--transition-normal);
}

.slide-up-enter-from,
.slide-up-leave-to {
  opacity: 0;
  transform: translateX(-50%) translateY(1rem);
}

/* ─── Responsive ─── */
@media (max-width: 560px) {
  .loading-state,
  .error-state {
    flex-direction: column;
    align-items: center;
    text-align: center;
  }

  .soft-error-bar {
    white-space: normal;
    text-align: center;
  }
}
</style>
