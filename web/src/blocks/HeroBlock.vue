<script setup lang="ts">
const props = withDefaults(
  defineProps<{
    eyebrow?: string;
    title?: string;
    subtitle?: string;
    primaryText?: string;
    primaryHref?: string;
    secondaryText?: string;
    secondaryHref?: string;
    image?: string;
  }>(),
  {
    eyebrow: "Release",
    title: "未配置标题",
    subtitle: "",
    primaryText: "了解更多",
    primaryHref: "/",
    secondaryText: "",
    secondaryHref: "",
    image: ""
  }
);
</script>

<template>
  <section class="hero">
    <div class="hero-copy">
      <p class="kicker">{{ eyebrow }}</p>
      <h2>{{ title }}</h2>
      <p>{{ subtitle }}</p>
      <div class="actions">
        <a :href="props.primaryHref">{{ primaryText }}</a>
        <a v-if="secondaryText && secondaryHref" class="secondary" :href="secondaryHref">
          {{ secondaryText }}
        </a>
      </div>
    </div>

    <img v-if="image" class="hero-image" :src="image" :alt="title" />
  </section>
</template>

<style scoped>
.hero {
  display: grid;
  grid-template-columns: minmax(0, 1fr) minmax(0, 1fr);
  gap: clamp(0.75rem, 2vw, 1.25rem);
  align-items: center;
  padding: clamp(0.75rem, 2vw, 1rem);
  border-radius: var(--radius-lg);
  border: 1px solid var(--line);
  background: linear-gradient(135deg, #13233d, #173f5f 56%, #0f766e);
  color: #f8fbff;
  box-shadow: var(--shadow);
  overflow: hidden;
}

.hero-copy {
  min-width: 0;
  padding: clamp(0.5rem, 1.5vw, 1rem);
}

.kicker {
  margin: 0 0 0.65rem;
  color: #9bd8c5;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  font-size: clamp(0.7rem, 1vw, 0.8rem);
  font-weight: 600;
}

.hero h2 {
  margin: 0;
  max-width: 13ch;
  font-size: clamp(1.6rem, 4vw + 0.3rem, 3.6rem);
  line-height: 1.08;
  overflow-wrap: anywhere;
  letter-spacing: -0.02em;
}

.hero p {
  max-width: 48rem;
  margin: 0.85rem 0 0;
  font-size: var(--font-body);
  color: rgba(248, 251, 255, 0.82);
  line-height: 1.75;
}

.actions {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-s);
  margin-top: clamp(1rem, 2vw, 1.5rem);
}

.actions a {
  display: inline-flex;
  justify-content: center;
  max-width: 100%;
  padding: 0.65rem 1rem;
  border-radius: var(--radius-md);
  background: #f8fbff;
  color: #13233d;
  text-align: center;
  font-weight: 700;
  font-size: var(--font-sm);
  transition: all var(--transition-normal);
}

.actions a:hover {
  transform: translateY(-1px);
  box-shadow: 0 8px 20px rgba(0, 0, 0, 0.15);
}

.actions a:active {
  transform: translateY(0);
}

.actions .secondary {
  border: 1px solid rgba(248, 251, 255, 0.38);
  background: transparent;
  color: #f8fbff;
}

.actions .secondary:hover {
  background: rgba(248, 251, 255, 0.1);
  box-shadow: none;
}

.hero-image {
  display: block;
  width: 100%;
  aspect-ratio: 16 / 10;
  object-fit: cover;
  object-position: top center;
  border-radius: var(--radius-sm);
  border: 1px solid rgba(248, 251, 255, 0.2);
  background: #eef2f7;
  transition: transform var(--transition-slow);
}

.hero:hover .hero-image {
  transform: scale(1.02);
}

@media (max-width: 860px) {
  .hero {
    grid-template-columns: 1fr;
  }
}

@media (max-width: 520px) {
  .hero {
    padding: 0.65rem;
  }

  .hero-copy {
    padding: 0.5rem;
  }

  .actions {
    align-items: stretch;
  }

  .actions a {
    width: 100%;
  }
}
</style>
