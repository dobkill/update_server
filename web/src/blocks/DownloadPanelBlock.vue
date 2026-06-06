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
    <div class="panel-info">
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
  gap: var(--space-m);
  padding: clamp(1rem, 2vw, 1.2rem);
  border-radius: var(--radius-lg);
  border: 1px solid var(--line);
  background: linear-gradient(135deg, #ffffff, #eef4ff);
  box-shadow: var(--shadow-soft);
}

.tag {
  margin: 0 0 0.4rem;
  color: var(--primary);
  font-size: var(--font-sm);
  font-weight: 600;
  letter-spacing: 0.03em;
}

.panel h3 {
  margin: 0;
  font-size: clamp(1.6rem, 3vw, 2.4rem);
  overflow-wrap: anywhere;
  letter-spacing: -0.02em;
}

.panel p {
  color: var(--muted);
  font-size: var(--font-body);
  overflow-wrap: anywhere;
  margin: 0;
}

.download-link {
  justify-self: start;
  display: inline-flex;
  align-items: center;
  gap: 0.3rem;
  padding: 0.7rem 1.2rem;
  border-radius: var(--radius-md);
  background: linear-gradient(135deg, var(--primary), #2466d8);
  color: #fff;
  font-weight: 700;
  font-size: var(--font-body);
  box-shadow: 0 6px 16px rgba(79, 70, 229, 0.25);
  transition: all var(--transition-normal);
  position: relative;
  overflow: hidden;
}

.download-link::after {
  content: "";
  position: absolute;
  inset: 0;
  border-radius: inherit;
  background: rgba(255, 255, 255, 0.15);
  opacity: 0;
  transition: opacity var(--transition-fast);
}

.download-link:hover {
  transform: translateY(-2px);
  box-shadow: 0 10px 24px rgba(79, 70, 229, 0.35);
}

.download-link:hover::after {
  opacity: 1;
}

.download-link:active {
  transform: translateY(0);
}

dl {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-m);
  margin: 0;
}

dl > div {
  padding: 0.45rem 0.65rem;
  border: 1px solid var(--line);
  border-radius: var(--radius-sm);
  background: rgba(255, 255, 255, 0.7);
}

dt {
  font-size: var(--font-sm);
  color: var(--muted);
  margin: 0;
}

dd {
  margin: 0.2rem 0 0;
  font-weight: 600;
  overflow-wrap: anywhere;
  font-size: var(--font-body);
}

@media (max-width: 520px) {
  .panel {
    padding: 0.85rem;
  }

  .panel h3 {
    font-size: 1.6rem;
  }

  .download-link {
    width: 100%;
    justify-self: stretch;
    text-align: center;
    justify-content: center;
  }
}
</style>
