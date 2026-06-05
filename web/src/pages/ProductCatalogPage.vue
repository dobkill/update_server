<script setup lang="ts">
import { computed } from "vue";
import type { ProductSummary } from "../runtime/api";

const props = defineProps<{
  items: ProductSummary[];
  keyword: string;
}>();

const emit = defineEmits<{
  "update:keyword": [value: string];
  open: [productCode: string];
}>();

const resultLabel = computed(() => `${props.items.length} 个项目`);

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
  <section class="catalog-toolbar">
    <div class="title-group">
      <p class="eyebrow">Release Catalog</p>
      <h1>产品与插件</h1>
      <span>{{ resultLabel }}</span>
    </div>

    <label class="search-panel" for="product-search">
      <span>搜索</span>
      <input
        id="product-search"
        :value="keyword"
        type="search"
        placeholder="项目名、编码或简介"
        @input="emit('update:keyword', ($event.target as HTMLInputElement).value)"
      />
    </label>
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
        <span v-if="item.latest_version" class="version-chip">{{ item.latest_version }}</span>
        <span v-if="item.updated_at">{{ item.updated_at.slice(0, 10) }}</span>
      </div>

      <button type="button" class="card-action" @click="emit('open', item.product_code)">
        打开详情
      </button>
    </article>
  </section>

  <section v-else class="empty-card">
    <h2>没有匹配到项目</h2>
    <p>换个关键词再试，或清空搜索条件查看完整列表。</p>
  </section>
</template>

<style scoped>
.catalog-toolbar {
  display: flex;
  justify-content: space-between;
  gap: 1rem;
  align-items: end;
  margin-top: 1rem;
  padding: 1.1rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.title-group {
  display: grid;
  gap: 0.25rem;
}

.eyebrow {
  margin: 0;
  color: var(--primary);
  letter-spacing: 0.08em;
  text-transform: uppercase;
  font-size: 0.78rem;
}

.catalog-toolbar h1 {
  margin: 0;
  font-size: 1.85rem;
  line-height: 1.1;
}

.title-group span,
.search-panel span {
  color: var(--muted);
  font-size: 0.9rem;
}

.search-panel {
  display: grid;
  gap: 0.35rem;
  min-width: min(24rem, 100%);
}

.search-panel input {
  width: 100%;
  padding: 0.7rem 0.8rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: #fbfcff;
}

.catalog-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(260px, 1fr));
  gap: 1rem;
  margin-top: 1rem;
}

.product-card,
.empty-card {
  display: grid;
  gap: 1rem;
  align-content: start;
  padding: 1rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.card-cover {
  aspect-ratio: 16 / 9;
  border-radius: 6px;
  overflow: hidden;
  background: linear-gradient(135deg, #e9f1ff, #e8f7f0 62%, #fff4dd);
}

.card-cover img {
  display: block;
  width: 100%;
  height: 100%;
  object-fit: cover;
}

.cover-fallback {
  display: grid;
  place-items: center;
  height: 100%;
  color: var(--primary-dark);
  font-size: 2.2rem;
  font-weight: 700;
}

.card-copy {
  display: grid;
  gap: 0.55rem;
}

.card-code {
  margin: 0;
  color: var(--primary);
  font-size: 0.82rem;
}

.card-copy h2 {
  margin: 0;
  font-size: 1.25rem;
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
  padding: 0.32rem 0.6rem;
  border-radius: 999px;
  background: var(--surface-subtle);
  color: var(--primary-dark);
}

.card-action {
  justify-self: start;
  padding: 0.65rem 0.85rem;
  border: 0;
  border-radius: 8px;
  background: var(--primary);
  color: #fff;
  cursor: pointer;
}

.empty-card {
  margin-top: 1rem;
}

.empty-card h2,
.empty-card p {
  margin: 0;
}

.empty-card p {
  color: var(--muted);
}

@media (max-width: 760px) {
  .catalog-toolbar {
    align-items: stretch;
    flex-direction: column;
  }
}
</style>
