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
  padding: 1.2rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.block h3 {
  margin: 0 0 0.75rem;
  font-size: 1.7rem;
  overflow-wrap: anywhere;
}

.block p {
  margin: 0;
  line-height: 1.8;
  color: var(--muted);
  overflow-wrap: anywhere;
}

@media (max-width: 520px) {
  .block {
    padding: 1rem;
  }

  .block h3 {
    font-size: 1.35rem;
  }
}
</style>
