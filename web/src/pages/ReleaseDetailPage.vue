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
    <button type="button" class="back-link" @click="emit('back')">返回</button>

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
  gap: 1rem;
  align-items: center;
  margin-top: 1rem;
  padding: 1rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.back-link {
  padding: 0.6rem 0.8rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: #fbfcff;
  color: var(--primary-dark);
  cursor: pointer;
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
  font-size: 0.82rem;
}

.release-title h1 {
  overflow-wrap: anywhere;
  font-size: 1.35rem;
}

.release-meta {
  display: flex;
  flex-wrap: wrap;
  gap: 0.65rem;
  min-width: 0;
}

.release-meta div {
  min-width: 5.4rem;
  padding: 0.52rem 0.65rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: var(--surface-subtle);
}

.release-meta dt {
  color: var(--muted);
  font-size: 0.74rem;
}

.release-meta dd {
  margin-top: 0.18rem;
  font-weight: 700;
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
    margin-top: 0.75rem;
    padding: 0.85rem;
  }

  .release-title h1 {
    font-size: 1.16rem;
  }
}
</style>
