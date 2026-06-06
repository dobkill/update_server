<script setup lang="ts">
withDefaults(
  defineProps<{
    title?: string;
    content?: string;
    image?: string;
  }>(),
  {
    title: "图文区块",
    content: "",
    image: ""
  }
);
</script>

<template>
  <section class="panel">
    <div class="copy">
      <h3>{{ title }}</h3>
      <p>{{ content }}</p>
    </div>
    <div class="art">
      <img v-if="image" :src="image" :alt="title" />
      <div v-else class="placeholder">可接入发布后的静态资源</div>
    </div>
  </section>
</template>

<style scoped>
.panel {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(min(100%, 260px), 1fr));
  gap: var(--space-m);
  padding: clamp(0.75rem, 1.5vw, 1rem);
  border: 1px solid var(--line);
  border-radius: var(--radius-md);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
  transition: box-shadow var(--transition-normal);
}

.panel:hover {
  box-shadow: var(--shadow-hover);
}

.copy h3 {
  margin: 0 0 0.5rem;
  font-size: var(--font-lg);
}

.copy p {
  margin: 0;
  line-height: 1.8;
  color: var(--muted);
  font-size: var(--font-body);
  overflow-wrap: anywhere;
}

.art {
  display: grid;
  place-items: center;
  min-height: 200px;
  border-radius: var(--radius-sm);
  background: var(--surface-subtle);
  overflow: hidden;
}

.art img {
  max-width: 100%;
  border-radius: var(--radius-sm);
  transition: transform var(--transition-slow);
}

.panel:hover .art img {
  transform: scale(1.03);
}

.placeholder {
  padding: 1rem;
  text-align: center;
  color: var(--muted);
  font-size: var(--font-sm);
}

@media (max-width: 520px) {
  .panel {
    padding: 0.75rem;
  }

  .art {
    min-height: 150px;
  }
}
</style>
