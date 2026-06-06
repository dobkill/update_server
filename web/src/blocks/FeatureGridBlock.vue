<script setup lang="ts">
defineProps<{
  items?: Array<{
    title: string;
    desc: string;
  }>;
}>();
</script>

<template>
  <section class="grid">
    <article v-for="item in items ?? []" :key="item.title" class="card">
      <h3>{{ item.title }}</h3>
      <p>{{ item.desc }}</p>
    </article>
  </section>
</template>

<style scoped>
.grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(min(100%, 220px), 1fr));
  gap: var(--space-m);
}

.card {
  padding: clamp(0.85rem, 1.5vw, 1.1rem);
  border: 1px solid var(--line);
  border-radius: var(--radius-md);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
  transition: transform var(--transition-normal), box-shadow var(--transition-normal), border-color var(--transition-normal);
}

.card:hover {
  transform: translateY(-3px);
  box-shadow: var(--shadow-hover);
  border-color: rgba(79, 70, 229, 0.2);
}

.card h3 {
  margin: 0 0 0.5rem;
  font-size: var(--font-lg);
}

.card p {
  margin: 0;
  color: var(--muted);
  font-size: var(--font-sm);
  line-height: 1.65;
  overflow-wrap: anywhere;
}
</style>
