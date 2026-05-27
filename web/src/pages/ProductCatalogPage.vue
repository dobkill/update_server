<script setup lang="ts">
import { computed } from "vue";
import type { ProductSummary } from "../runtime/api";

const props = defineProps<{
  items: ProductSummary[];
  keyword: string;
  dataSource: string;
}>();

const emit = defineEmits<{
  "update:keyword": [value: string];
  open: [productCode: string];
}>();

const resultLabel = computed(() => `${props.items.length} 个小项目`);

function productInitial(name: string): string {
  return name
    .split(/\s+/)
    .filter(Boolean)
    .slice(0, 2)
    .map((part) => part[0]?.toUpperCase() ?? "")
    .join("");
}
</script>

<template>
  <section class="catalog-hero">
    <div>
      <p class="eyebrow">Web Module / Product Catalog</p>
      <h1>所有小项目都先落在同一个默认页面</h1>
      <p class="summary">
        无参访问时展示统一产品列表。页面结构固定，数据来自 `GET /api/v1/products`，搜索在前端基于已加载列表完成。
      </p>
    </div>

    <div class="search-panel">
      <label class="search-label" for="product-search">搜索小项目</label>
      <input
        id="product-search"
        :value="keyword"
        type="search"
        placeholder="输入项目名、编码或简介"
        @input="emit('update:keyword', ($event.target as HTMLInputElement).value)"
      />
      <div class="meta-row">
        <strong>{{ resultLabel }}</strong>
        <span>数据来源：{{ dataSource }}</span>
      </div>
    </div>
  </section>

  <section v-if="items.length" class="catalog-grid">
    <article v-for="item in items" :key="item.product_code" class="product-card">
      <div class="card-cover">
        <img v-if="item.cover_image_url" :src="item.cover_image_url" :alt="item.name" />
        <div v-else class="cover-fallback">{{ productInitial(item.name) }}</div>
      </div>

      <div class="card-copy">
        <p class="card-code">{{ item.product_code }}</p>
        <h2>{{ item.name }}</h2>
        <p class="card-summary">{{ item.summary }}</p>
      </div>

      <div class="card-meta">
        <span v-if="item.latest_version" class="version-chip">Latest {{ item.latest_version }}</span>
        <span v-if="item.updated_at">{{ item.updated_at.slice(0, 10) }}</span>
      </div>

      <button type="button" class="card-action" @click="emit('open', item.product_code)">
        查看最新版本
      </button>
    </article>
  </section>

  <section v-else class="empty-card">
    <h2>没有匹配到小项目</h2>
    <p>可以换个关键词再试，或者清空搜索条件查看完整列表。</p>
  </section>
</template>

<style scoped>
.catalog-hero {
  display: grid;
  grid-template-columns: minmax(0, 1.4fr) minmax(280px, 0.8fr);
  gap: 1.25rem;
  margin-top: 1rem;
}

.eyebrow {
  margin: 0 0 0.75rem;
  color: var(--primary);
  letter-spacing: 0.12em;
  text-transform: uppercase;
  font-size: 0.78rem;
}

.catalog-hero h1 {
  margin: 0;
  max-width: 11ch;
  font-size: clamp(2.5rem, 6vw, 4.8rem);
  line-height: 0.94;
}

.summary {
  margin: 1rem 0 0;
  max-width: 56rem;
  color: var(--muted);
  line-height: 1.8;
}

.search-panel {
  display: grid;
  gap: 0.8rem;
  align-content: start;
  padding: 1.4rem;
  border: 1px solid var(--line);
  border-radius: 28px;
  background: rgba(255, 251, 244, 0.84);
  box-shadow: var(--shadow-soft);
}

.search-label {
  font-size: 0.9rem;
  color: var(--muted);
}

.search-panel input {
  width: 100%;
  padding: 0.95rem 1rem;
  border: 1px solid rgba(32, 29, 24, 0.15);
  border-radius: 18px;
  background: #fffdfa;
  font: inherit;
}

.meta-row {
  display: flex;
  justify-content: space-between;
  gap: 1rem;
  color: var(--muted);
  font-size: 0.9rem;
}

.catalog-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
  gap: 1rem;
  margin-top: 1.5rem;
}

.product-card,
.empty-card {
  display: grid;
  gap: 1rem;
  padding: 1.35rem;
  border: 1px solid var(--line);
  border-radius: 28px;
  background: rgba(255, 251, 244, 0.84);
  box-shadow: var(--shadow-soft);
}

.card-cover {
  min-height: 150px;
  border-radius: 22px;
  overflow: hidden;
  background:
    radial-gradient(circle at top left, rgba(188, 84, 47, 0.28), transparent 28%),
    linear-gradient(135deg, rgba(67, 120, 97, 0.18), rgba(226, 206, 176, 0.62));
}

.card-cover img {
  width: 100%;
  height: 100%;
  object-fit: cover;
  display: block;
}

.cover-fallback {
  display: grid;
  place-items: center;
  min-height: 150px;
  color: var(--primary-dark);
  font-size: 2.2rem;
  letter-spacing: 0.08em;
}

.card-copy {
  display: grid;
  gap: 0.55rem;
}

.card-code {
  margin: 0;
  color: var(--primary);
  font-size: 0.82rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}

.card-copy h2 {
  margin: 0;
  font-size: 1.5rem;
}

.card-summary {
  margin: 0;
  color: var(--muted);
  line-height: 1.7;
}

.card-meta {
  display: flex;
  justify-content: space-between;
  gap: 0.8rem;
  align-items: center;
  color: var(--muted);
  font-size: 0.88rem;
}

.version-chip {
  padding: 0.35rem 0.7rem;
  border-radius: 999px;
  background: rgba(188, 84, 47, 0.12);
  color: var(--primary-dark);
}

.card-action {
  justify-self: start;
  padding: 0.85rem 1.05rem;
  border: 0;
  border-radius: 999px;
  background: var(--primary);
  color: #fff8f1;
  cursor: pointer;
}

.empty-card {
  margin-top: 1.5rem;
}

.empty-card h2,
.empty-card p {
  margin: 0;
}

.empty-card p {
  color: var(--muted);
}

@media (max-width: 760px) {
  .catalog-hero {
    grid-template-columns: 1fr;
  }
}
</style>
