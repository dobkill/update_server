<script setup lang="ts">
defineProps<{
  items?: Array<{
    question: string;
    answer: string;
  }>;
}>();
</script>

<template>
  <section class="faq">
    <article v-for="item in items ?? []" :key="item.question" class="faq-item">
      <h3>{{ item.question }}</h3>
      <p>{{ item.answer }}</p>
    </article>
  </section>
</template>

<style scoped>
.faq {
  display: grid;
  gap: var(--space-s);
}

.faq-item {
  padding: clamp(0.75rem, 1.5vw, 1rem);
  border-radius: var(--radius-md);
  border: 1px solid var(--line);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
  transition: background var(--transition-fast), border-color var(--transition-fast);
}

.faq-item:hover {
  background: #fafaff;
  border-color: rgba(79, 70, 229, 0.15);
}

.faq h3 {
  margin: 0 0 0.6rem;
  font-size: var(--font-lg);
  overflow-wrap: anywhere;
  color: var(--text);
}

.faq p {
  margin: 0;
  color: var(--muted);
  font-size: var(--font-body);
  line-height: 1.75;
  overflow-wrap: anywhere;
}
</style>
