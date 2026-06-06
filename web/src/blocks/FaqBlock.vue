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
    <article v-for="item in items ?? []" :key="item.question">
      <h3>{{ item.question }}</h3>
      <p>{{ item.answer }}</p>
    </article>
  </section>
</template>

<style scoped>
.faq {
  display: grid;
  gap: 1rem;
}

.faq article {
  padding: 1rem;
  border-radius: 8px;
  border: 1px solid var(--line);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.faq h3 {
  margin: 0 0 0.75rem;
  overflow-wrap: anywhere;
}

.faq p {
  margin: 0;
  color: var(--muted);
  line-height: 1.7;
  overflow-wrap: anywhere;
}
</style>
