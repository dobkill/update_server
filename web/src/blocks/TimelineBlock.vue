<script setup lang="ts">
defineProps<{
  items?: string[];
}>();
</script>

<template>
  <section class="timeline">
    <div v-for="(item, index) in items ?? []" :key="item" class="row">
      <span>{{ index + 1 }}</span>
      <p>{{ item }}</p>
    </div>
  </section>
</template>

<style scoped>
.timeline {
  display: grid;
  gap: 0.85rem;
}

.row {
  display: grid;
  grid-template-columns: 3rem 1fr;
  gap: 1rem;
  align-items: start;
  padding: 1rem 1.25rem;
  border-radius: 8px;
  border: 1px solid var(--line);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.row span {
  display: inline-grid;
  place-items: center;
  width: 2rem;
  height: 2rem;
  border-radius: 999px;
  background: rgba(178, 76, 42, 0.12);
  color: var(--primary);
}

.row p {
  margin: 0;
  color: var(--muted);
}
</style>
