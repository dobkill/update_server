<script setup lang="ts">
const props = withDefaults(
  defineProps<{
    title?: string;
    subtitle?: string;
    primaryText?: string;
    primaryHref?: string;
  }>(),
  {
    title: "未配置标题",
    subtitle: "",
    primaryText: "了解更多",
    primaryHref: "/"
  }
);
</script>

<template>
  <section class="hero">
    <p class="kicker">动态页面区块</p>
    <h2>{{ title }}</h2>
    <p>{{ subtitle }}</p>
    <a :href="props.primaryHref">{{ primaryText }}</a>
  </section>
</template>

<style scoped>
.hero {
  padding: 2.5rem;
  border-radius: 28px;
  background: linear-gradient(135deg, rgba(178, 76, 42, 0.95), rgba(120, 54, 23, 0.92));
  color: #fff8f1;
  box-shadow: var(--shadow);
}

.kicker {
  margin: 0 0 0.75rem;
  opacity: 0.76;
  letter-spacing: 0.12em;
  text-transform: uppercase;
  font-size: 0.8rem;
}

.hero h2 {
  margin: 0;
  max-width: 14ch;
  font-size: clamp(2.2rem, 5vw, 4.4rem);
  line-height: 1.02;
}

.hero p {
  max-width: 48rem;
  margin: 1rem 0 0;
  font-size: 1.05rem;
}

.hero a {
  display: inline-flex;
  margin-top: 1.5rem;
  padding: 0.85rem 1.2rem;
  border-radius: 999px;
  background: rgba(255, 248, 241, 0.16);
}
</style>
