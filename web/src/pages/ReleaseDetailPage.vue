<script setup lang="ts">
import { computed } from "vue";
import BlockRenderer from "../runtime/BlockRenderer.vue";
import { type PageBlock, type ProductReleaseDetail } from "../runtime/api";

const props = defineProps<{
  release: ProductReleaseDetail;
  blocks: PageBlock[];
}>();

const emit = defineEmits<{
  back: [];
}>();

const publishedDate = computed(() => props.release.published_at?.slice(0, 10) ?? "--");
</script>

<template>
  <section class="release-toolbar">
    <button type="button" class="back-link" @click="emit('back')">
      <span aria-hidden="true">←</span> 返回
    </button>

    <div class="release-title">
      <p>{{ release.product_code }}</p>
      <h1>{{ release.title }}</h1>
    </div>

    <dl class="release-meta">
      <div>
        <dt>版本</dt>
        <dd>{{ release.resolved_version }}</dd>
      </div>
      <div>
        <dt>渠道</dt>
        <dd>{{ release.channel }}</dd>
      </div>
      <div>
        <dt>发布</dt>
        <dd>{{ publishedDate }}</dd>
      </div>
    </dl>
  </section>

  <BlockRenderer :blocks="blocks" />
</template>

<style scoped>
.release-toolbar {
  display: grid;
  grid-template-columns: auto minmax(0, 1fr) auto;
  gap: var(--space-m);
  align-items: center;
  margin-top: var(--space-m);
  padding: clamp(0.75rem, 1.5vw, 1rem);
  border: 1px solid var(--line);
  border-radius: var(--radius-lg);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.back-link {
  display: inline-flex;
  align-items: center;
  gap: 0.3rem;
  padding: 0.5rem 0.85rem;
  border: 1px solid var(--line);
  border-radius: var(--radius-sm);
  background: #fbfcff;
  color: var(--primary-dark);
  font-size: var(--font-sm);
  font-weight: 600;
  cursor: pointer;
  transition: all var(--transition-fast);
  white-space: nowrap;
}

.back-link:hover {
  background: var(--surface-subtle);
  border-color: var(--primary-light);
  color: var(--primary);
}

.back-link:active {
  transform: scale(0.97);
}

.release-title {
  min-width: 0;
}

.release-title p,
.release-title h1,
.release-meta,
.release-meta dt,
.release-meta dd {
  margin: 0;
}

.release-title p {
  color: var(--primary);
  font-size: var(--font-sm);
  font-weight: 600;
  letter-spacing: 0.03em;
}

.release-title h1 {
  overflow-wrap: anywhere;
  font-size: var(--font-xl);
  letter-spacing: -0.01em;
  line-height: 1.25;
}

.release-meta {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-xs);
  min-width: 0;
}

.release-meta div {
  min-width: 5rem;
  padding: 0.4rem 0.6rem;
  border: 1px solid var(--line);
  border-radius: var(--radius-sm);
  background: var(--surface-subtle);
  transition: border-color var(--transition-fast);
}

.release-meta div:hover {
  border-color: rgba(79, 70, 229, 0.2);
}

.release-meta dt {
  color: var(--muted);
  font-size: clamp(0.68rem, 0.8vw, 0.74rem);
  font-weight: 500;
}

.release-meta dd {
  margin-top: 0.15rem;
  font-weight: 700;
  font-size: var(--font-sm);
}

@media (max-width: 820px) {
  .release-toolbar {
    grid-template-columns: 1fr;
    align-items: stretch;
  }

  .back-link {
    justify-self: start;
  }

  .release-meta {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(min(100%, 8rem), 1fr));
  }
}

@media (max-width: 520px) {
  .release-toolbar {
    margin-top: var(--space-s);
    padding: 0.75rem;
  }

  .release-title h1 {
    font-size: var(--font-lg);
  }
}
</style>
