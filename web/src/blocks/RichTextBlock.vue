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
  padding: clamp(0.85rem, 1.5vw, 1.2rem);
  padding-left: clamp(1rem, 2vw, 1.4rem);
  border: 1px solid var(--line);
  border-radius: var(--radius-md);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
  position: relative;
  overflow: hidden;
}

.block::before {
  content: "";
  position: absolute;
  top: 0;
  left: 0;
  width: 3.5px;
  height: 100%;
  background: linear-gradient(180deg, var(--primary), var(--accent));
  border-radius: 999px;
}

.block h3 {
  margin: 0 0 0.65rem;
  font-size: var(--font-xl);
  overflow-wrap: anywhere;
}

.block p {
  margin: 0;
  line-height: 1.8;
  color: var(--muted);
  font-size: var(--font-body);
  overflow-wrap: anywhere;
}

@media (max-width: 520px) {
  .block {
    padding: 0.85rem;
    padding-left: 1rem;
  }

  .block h3 {
    font-size: var(--font-lg);
  }
}
</style>
