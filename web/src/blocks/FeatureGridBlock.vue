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
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap: 1rem;
}

.card {
  padding: 1.5rem;
  border: 1px solid var(--line);
  border-radius: 24px;
  background: var(--surface);
}

.card h3 {
  margin: 0 0 0.5rem;
}

.card p {
  margin: 0;
  color: var(--muted);
}
</style>
