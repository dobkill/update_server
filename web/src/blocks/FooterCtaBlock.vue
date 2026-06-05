<script setup lang="ts">
const props = withDefaults(
  defineProps<{
    title?: string;
    desc?: string;
    actionText?: string;
    actionHref?: string;
  }>(),
  {
    title: "继续扩展页面能力",
    desc: "",
    actionText: "前往",
    actionHref: "/"
  }
);
</script>

<template>
  <section class="cta">
    <div>
      <h3>{{ title }}</h3>
      <p>{{ desc }}</p>
    </div>
    <a :href="props.actionHref">{{ actionText }}</a>
  </section>
</template>

<style scoped>
.cta {
  display: flex;
  justify-content: space-between;
  gap: 1rem;
  align-items: center;
  padding: 1.2rem;
  border-radius: 8px;
  background: #151a23;
  color: #f8fbff;
}

.cta h3 {
  margin: 0 0 0.5rem;
}

.cta p {
  margin: 0;
  color: rgba(247, 242, 232, 0.76);
}

.cta a {
  padding: 0.75rem 0.95rem;
  border-radius: 8px;
  background: var(--primary);
  white-space: nowrap;
}

@media (max-width: 700px) {
  .cta {
    flex-direction: column;
    align-items: flex-start;
  }
}
</style>
