<script setup lang="ts">
defineProps<{
  title?: string;
  intro?: string;
  items?: Array<{
    title: string;
    desc?: string;
    image: string;
  }>;
}>();
</script>

<template>
  <section class="gallery">
    <div class="gallery-head">
      <h3>{{ title }}</h3>
      <p v-if="intro">{{ intro }}</p>
    </div>

    <div class="gallery-grid">
      <article v-for="item in items ?? []" :key="item.title" class="shot-card">
        <img :src="item.image" :alt="item.title" loading="lazy" />
        <div>
          <h4>{{ item.title }}</h4>
          <p v-if="item.desc">{{ item.desc }}</p>
        </div>
      </article>
    </div>
  </section>
</template>

<style scoped>
.gallery {
  display: grid;
  gap: 1rem;
}

.gallery-head {
  display: grid;
  gap: 0.45rem;
}

.gallery-head h3,
.gallery-head p,
.shot-card h4,
.shot-card p {
  margin: 0;
}

.gallery-head h3 {
  font-size: 1.55rem;
}

.gallery-head p,
.shot-card p {
  color: var(--muted);
  line-height: 1.65;
}

.gallery-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(min(100%, 280px), 1fr));
  gap: 1rem;
}

.shot-card {
  display: grid;
  gap: 0.85rem;
  min-width: 0;
  padding: 0.8rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.shot-card img {
  display: block;
  width: 100%;
  aspect-ratio: 16 / 9;
  object-fit: cover;
  object-position: top center;
  border-radius: 6px;
  border: 1px solid var(--line);
  background: #eef2f7;
}

.shot-card h4 {
  font-size: 1rem;
  overflow-wrap: anywhere;
}

@media (max-width: 520px) {
  .gallery-head h3 {
    font-size: 1.3rem;
  }
}
</style>
