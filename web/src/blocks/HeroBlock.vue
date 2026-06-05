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
  grid-template-columns: minmax(0, 0.86fr) minmax(260px, 1.14fr);
  gap: 1.25rem;
  align-items: center;
  padding: 1rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: linear-gradient(135deg, #13233d, #173f5f 56%, #0f766e);
  color: #f8fbff;
  box-shadow: var(--shadow);
}

.hero-copy {
  padding: 1rem;
}

.kicker {
  margin: 0 0 0.75rem;
  color: #9bd8c5;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  font-size: 0.8rem;
}

.hero h2 {
  margin: 0;
  max-width: 13ch;
  font-size: 3.6rem;
  line-height: 1.04;
}

.hero p {
  max-width: 48rem;
  margin: 1rem 0 0;
  font-size: 1.05rem;
  color: rgba(248, 251, 255, 0.82);
  line-height: 1.75;
}

.actions {
  display: flex;
  flex-wrap: wrap;
  gap: 0.7rem;
  margin-top: 1.5rem;
}

.actions a {
  display: inline-flex;
  padding: 0.75rem 0.95rem;
  border-radius: 8px;
  background: #f8fbff;
  color: #13233d;
  font-weight: 700;
}

.actions .secondary {
  border: 1px solid rgba(248, 251, 255, 0.38);
  background: transparent;
  color: #f8fbff;
}

.hero-image {
  display: block;
  width: 100%;
  aspect-ratio: 16 / 10;
  object-fit: cover;
  object-position: top center;
  border-radius: 6px;
  border: 1px solid rgba(248, 251, 255, 0.2);
  background: #eef2f7;
}

@media (max-width: 860px) {
  .hero {
    grid-template-columns: 1fr;
  }

  .hero h2 {
    font-size: 2.2rem;
  }
}
</style>
