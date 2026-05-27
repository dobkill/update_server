<script setup lang="ts">
import { computed } from "vue";
import BlockRenderer from "../runtime/BlockRenderer.vue";
import { type PageBlock, type ProductReleaseDetail } from "../runtime/api";

const props = defineProps<{
  release: ProductReleaseDetail;
  blocks: PageBlock[];
  dataSource: string;
}>();

const emit = defineEmits<{
  back: [];
}>();

const isLatestAlias = computed(() => props.release.requested_version === "latest");
const publishedDate = computed(() => props.release.published_at?.slice(0, 10) ?? "--");
</script>

<template>
  <section class="release-hero">
    <div class="hero-copy">
      <button type="button" class="back-link" @click="emit('back')">返回产品列表</button>
      <p class="eyebrow">{{ release.product_code }}</p>
      <h1>{{ release.title }}</h1>
      <p class="summary">{{ release.release_notes_summary }}</p>
    </div>

    <aside class="meta-panel">
      <div>
        <p class="meta-label">请求版本</p>
        <strong>{{ release.requested_version }}</strong>
      </div>
      <div>
        <p class="meta-label">实际版本</p>
        <strong>{{ release.resolved_version }}</strong>
      </div>
      <div>
        <p class="meta-label">渠道</p>
        <strong>{{ release.channel }}</strong>
      </div>
      <div>
        <p class="meta-label">发布时间</p>
        <strong>{{ publishedDate }}</strong>
      </div>
      <div>
        <p class="meta-label">页面数据</p>
        <strong>{{ dataSource }}</strong>
      </div>
      <div v-if="release.page.vue_entry_url">
        <p class="meta-label">入口脚本</p>
        <code>{{ release.page.vue_entry_url }}</code>
      </div>
      <p v-if="isLatestAlias" class="hint">
        当前访问的是默认详情页，web 已请求 `Document?version=latest`，并展示当前渠道下解析到的实际版本。
      </p>
    </aside>
  </section>

  <BlockRenderer :blocks="blocks" />
</template>

<style scoped>
.release-hero {
  display: grid;
  grid-template-columns: minmax(0, 1.35fr) minmax(280px, 0.75fr);
  gap: 1.25rem;
  margin-top: 1rem;
}

.hero-copy,
.meta-panel {
  padding: 1.6rem;
  border: 1px solid var(--line);
  border-radius: 28px;
  background: rgba(255, 251, 244, 0.84);
  box-shadow: var(--shadow-soft);
}

.back-link {
  justify-self: start;
  margin-bottom: 1rem;
  padding: 0;
  border: 0;
  background: transparent;
  color: var(--primary-dark);
  cursor: pointer;
}

.eyebrow {
  margin: 0 0 0.75rem;
  color: var(--primary);
  letter-spacing: 0.1em;
  text-transform: uppercase;
  font-size: 0.8rem;
}

.hero-copy h1 {
  margin: 0;
  font-size: clamp(2.4rem, 5vw, 4.6rem);
  line-height: 0.96;
}

.summary {
  margin: 1rem 0 0;
  color: var(--muted);
  line-height: 1.8;
}

.meta-panel {
  display: grid;
  gap: 1rem;
  align-content: start;
}

.meta-label {
  margin: 0 0 0.35rem;
  color: var(--muted);
  font-size: 0.82rem;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}

.meta-panel strong,
.meta-panel code {
  font-size: 0.98rem;
}

.meta-panel code {
  display: block;
  word-break: break-all;
  color: var(--primary-dark);
}

.hint {
  margin: 0;
  color: var(--muted);
  line-height: 1.7;
}

@media (max-width: 760px) {
  .release-hero {
    grid-template-columns: 1fr;
  }
}
</style>
