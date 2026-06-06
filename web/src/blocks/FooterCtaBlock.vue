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
    <div class="cta-copy">
      <h3>{{ title }}</h3>
      <p>{{ desc }}</p>
    </div>
    <a class="cta-button" :href="props.actionHref">{{ actionText }}</a>
  </section>
</template>

<style scoped>
.cta {
  display: flex;
  justify-content: space-between;
  gap: var(--space-m);
  align-items: center;
  padding: clamp(1rem, 2vw, 1.2rem);
  border-radius: var(--radius-lg);
  background: linear-gradient(135deg, #151a23, #1a2332);
  color: #f8fbff;
  transition: box-shadow var(--transition-normal);
}

.cta:hover {
  box-shadow: 0 8px 28px rgba(21, 26, 35, 0.25);
}

.cta-copy h3 {
  margin: 0 0 0.4rem;
  font-size: var(--font-lg);
}

.cta-copy p {
  margin: 0;
  color: rgba(247, 242, 232, 0.76);
  font-size: var(--font-sm);
  overflow-wrap: anywhere;
  line-height: 1.6;
}

.cta-button {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  max-width: 100%;
  padding: 0.7rem 1.1rem;
  border-radius: var(--radius-md);
  background: var(--primary);
  color: #fff;
  text-align: center;
  white-space: nowrap;
  font-weight: 700;
  font-size: var(--font-sm);
  transition: all var(--transition-normal);
  box-shadow: 0 4px 12px rgba(79, 70, 229, 0.3);
}

.cta-button:hover {
  background: var(--primary-light);
  transform: translateY(-1px);
  box-shadow: 0 8px 20px rgba(79, 70, 229, 0.4);
  color: #fff;
}

@media (max-width: 700px) {
  .cta {
    flex-direction: column;
    align-items: flex-start;
  }

  .cta-button {
    white-space: normal;
    align-self: flex-start;
  }
}
</style>
