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
  padding: 1.5rem;
  border-radius: 24px;
  border: 1px solid var(--line);
  background: var(--surface);
}

.faq h3 {
  margin: 0 0 0.75rem;
}

.faq p {
  margin: 0;
  color: var(--muted);
  line-height: 1.7;
}
</style>
