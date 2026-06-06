<script setup lang="ts">
defineProps<{
  items?: string[];
}>();
</script>

<template>
  <section class="timeline">
    <div v-for="(item, index) in items ?? []" :key="item" class="row">
      <span class="step-num">{{ index + 1 }}</span>
      <p>{{ item }}</p>
    </div>
  </section>
</template>

<style scoped>
.timeline {
  display: grid;
  gap: 0.75rem;
}

.row {
  display: grid;
  grid-template-columns: 2.8rem minmax(0, 1fr);
  gap: 0.85rem;
  align-items: start;
  padding: clamp(0.75rem, 1.5vw, 1rem) clamp(0.85rem, 1.5vw, 1.25rem);
  border-radius: var(--radius-md);
  border: 1px solid var(--line);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
  transition: transform var(--transition-normal), box-shadow var(--transition-normal), border-color var(--transition-normal);
}

.row:hover {
  transform: translateX(4px);
  box-shadow: var(--shadow-hover);
  border-color: rgba(79, 70, 229, 0.18);
}

.step-num {
  display: inline-grid;
  place-items: center;
  width: 2rem;
  height: 2rem;
  border-radius: 50%;
  background: linear-gradient(135deg, var(--primary), #2466d8);
  color: #fff;
  font-size: 0.8rem;
  font-weight: 700;
  box-shadow: 0 4px 10px rgba(79, 70, 229, 0.2);
}

.row p {
  margin: 0;
  color: var(--muted);
  font-size: var(--font-body);
  overflow-wrap: anywhere;
  line-height: 1.65;
}

@media (max-width: 520px) {
  .row {
    grid-template-columns: 2.2rem minmax(0, 1fr);
    gap: 0.65rem;
    padding: 0.75rem;
  }

  .step-num {
    width: 1.75rem;
    height: 1.75rem;
    font-size: 0.72rem;
  }
}
</style>
