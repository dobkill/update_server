<script setup lang="ts">
withDefaults(
  defineProps<{
    version?: string;
    packageType?: string;
    packageSize?: string;
    releaseNote?: string;
    downloadText?: string;
    downloadUrl?: string;
  }>(),
  {
    version: "unknown",
    packageType: "full",
    packageSize: "0 MB",
    releaseNote: "",
    downloadText: "立即下载",
    downloadUrl: ""
  }
);
</script>

<template>
  <section class="panel">
    <div>
      <p class="tag">当前推荐版本</p>
      <h3>{{ version }}</h3>
      <p>{{ releaseNote }}</p>
    </div>
    <dl>
      <div>
        <dt>包类型</dt>
        <dd>{{ packageType }}</dd>
      </div>
      <div>
        <dt>体积</dt>
        <dd>{{ packageSize }}</dd>
      </div>
    </dl>
    <a v-if="downloadUrl" class="download-link" :href="downloadUrl">{{ downloadText }}</a>
  </section>
</template>

<style scoped>
.panel {
  display: grid;
  gap: 1rem;
  padding: 1.2rem;
  border-radius: 8px;
  border: 1px solid var(--line);
  background: linear-gradient(135deg, #ffffff, #eef4ff);
  box-shadow: var(--shadow-soft);
}

.tag {
  margin: 0 0 0.5rem;
  color: var(--primary);
}

.panel h3 {
  margin: 0;
  font-size: 2.4rem;
  overflow-wrap: anywhere;
}

.panel p {
  color: var(--muted);
  overflow-wrap: anywhere;
}

.download-link {
  justify-self: start;
  padding: 0.75rem 0.95rem;
  border-radius: 8px;
  background: var(--primary);
  color: #fff;
}

dl {
  display: flex;
  flex-wrap: wrap;
  gap: 1rem;
  margin: 0;
}

dt {
  font-size: 0.85rem;
  color: var(--muted);
}

dd {
  margin: 0.25rem 0 0;
  font-weight: 600;
  overflow-wrap: anywhere;
}

@media (max-width: 520px) {
  .panel {
    padding: 1rem;
  }

  .panel h3 {
    font-size: 1.75rem;
  }

  .download-link {
    width: 100%;
    text-align: center;
  }
}
</style>
