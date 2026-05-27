<script setup lang="ts">
withDefaults(
  defineProps<{
    title?: string;
    content?: string;
  }>(),
  {
    title: "说明",
    content: ""
  }
);
</script>

<template>
  <section class="block">
    <h3>{{ title }}</h3>
    <p>{{ content }}</p>
  </section>
</template>

<style scoped>
.block {
  padding: 1.75rem;
  border: 1px solid var(--line);
  border-radius: 24px;
  background: var(--surface);
}

.block h3 {
  margin: 0 0 0.75rem;
  font-size: 1.7rem;
}

.block p {
  margin: 0;
  line-height: 1.8;
  color: var(--muted);
}
</style>
