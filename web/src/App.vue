<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref, watchEffect } from "vue";
import ProductCatalogPage from "./pages/ProductCatalogPage.vue";
import ReleaseDetailPage from "./pages/ReleaseDetailPage.vue";
import { composeReleaseBlocks } from "./runtime/pageComposer";
import {
  defaultReleaseChannel,
  fetchProducts,
  fetchReleaseDetail,
  type ProductReleaseDetail,
  type ProductSummary
} from "./runtime/api";
import { getMockProducts, getMockReleaseDetail } from "./runtime/mockData";
import {
  buildCatalogUrl,
  buildReleaseUrl,
  getCurrentAppLocation,
  latestVersionAlias
} from "./runtime/location";
import AppShell from "./shell/AppShell.vue";

const currentLocation = ref(getCurrentAppLocation());
const products = ref<ProductSummary[]>([]);
const release = ref<ProductReleaseDetail | null>(null);
const keyword = ref("");
const loading = ref(true);
const error = ref("");
const pageSource = ref("本地 JSON");

const title = computed(() => {
  if (currentLocation.value.page === "catalog") {
    return "小项目列表 | 更新平台 Web 模块";
  }

  return release.value
    ? `${release.value.title} | ${release.value.product_code}`
    : "版本详情 | 更新平台 Web 模块";
});

const filteredProducts = computed(() => {
  const needle = keyword.value.trim().toLowerCase();
  if (!needle) {
    return products.value;
  }

  return products.value.filter((item) =>
    [item.product_code, item.name, item.summary]
      .join(" ")
      .toLowerCase()
      .includes(needle)
  );
});

const releaseBlocks = computed(() => (release.value ? composeReleaseBlocks(release.value) : []));
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

  const remoteProducts = await fetchProducts();
  if (remoteProducts) {
    products.value = remoteProducts;
    pageSource.value = "API /api/v1/products";
    loading.value = false;
    return;
  }

  products.value = getMockProducts();
  pageSource.value = "本地 JSON";
  error.value = "产品列表接口暂不可用，当前展示的是本地 JSON 中的小项目数据。";
  loading.value = false;
}

function getReleaseSourceLabel(version: string, channel: string): string {
  return `API /api/v1/products/{product_code}/Document?version=${version}&channel=${channel}`;
}

async function loadReleasePage(productCode: string, version: string, channel: string) {
  loading.value = true;
  error.value = "";
  products.value = [];

  const remoteRelease = await fetchReleaseDetail(productCode, version, channel);
  if (remoteRelease) {
    release.value = remoteRelease;
    pageSource.value = getReleaseSourceLabel(version, channel);
    loading.value = false;
    return;
  }

  const fallbackRelease = getMockReleaseDetail(productCode, version, channel);
  if (fallbackRelease) {
    release.value = fallbackRelease;
    pageSource.value = "本地 JSON";
    error.value = `版本详情接口暂不可用，当前展示的是本地 JSON 中 ${channel} 渠道的页面装配数据。`;
  } else {
    release.value = null;
    pageSource.value = "无可用数据";
    error.value = `未找到产品 ${productCode} 在 ${channel} 渠道下的版本 ${version} 页面数据。`;
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
    @home="openCatalog"
  >
    <section class="status-band">
      <div>
        <p class="band-label">当前实现状态</p>
        <strong>已切换到产品列表 + 版本详情双页面</strong>
      </div>
      <div>
        <p class="band-label">当前页面</p>
        <strong>{{ currentLocation.page === "catalog" ? "默认产品列表页" : "小项目详情页" }}</strong>
      </div>
      <div>
        <p class="band-label">页面数据来源</p>
        <strong>{{ pageSource }}</strong>
      </div>
      <div v-if="currentLocation.page === 'release'">
        <p class="band-label">当前渠道</p>
        <strong>{{ currentChannel }}</strong>
      </div>
    </section>

    <section v-if="loading" class="state-card">
      <h1>正在装配页面</h1>
      <p>`web` 模块正在读取产品列表、版本参数和页面 JSON。</p>
    </section>

    <section v-else-if="error" class="state-card">
      <h1>已切换到后备数据</h1>
      <p>{{ error }}</p>
    </section>

    <ProductCatalogPage
      v-if="!loading && currentLocation.page === 'catalog'"
      :items="filteredProducts"
      :keyword="keyword"
      :data-source="pageSource"
      @update:keyword="keyword = $event"
      @open="openProduct"
    />

    <ReleaseDetailPage
      v-else-if="!loading && release && currentLocation.page === 'release'"
      :release="release"
      :blocks="releaseBlocks"
      :data-source="pageSource"
      @back="openCatalog"
    />
  </AppShell>
</template>

<style scoped>
.status-band {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 1rem;
  margin: 1.5rem auto 0;
}

.status-band > div {
  padding: 1rem 1.1rem;
  border: 1px solid var(--line);
  border-radius: 22px;
  background: rgba(255, 250, 242, 0.82);
  box-shadow: var(--shadow-soft);
}

.band-label {
  margin: 0 0 0.45rem;
  color: var(--muted);
  font-size: 0.82rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}

.status-band strong {
  font-size: 1rem;
}

.state-card {
  margin: 1.5rem auto 0;
  max-width: 56rem;
  padding: 1.6rem 1.8rem;
  border: 1px solid var(--line);
  border-radius: 26px;
  background: rgba(255, 250, 242, 0.9);
  box-shadow: var(--shadow-soft);
}

.state-card h1 {
  margin: 0 0 0.75rem;
  font-size: clamp(1.8rem, 4vw, 2.5rem);
}

.state-card p {
  margin: 0;
  color: var(--muted);
}
</style>
