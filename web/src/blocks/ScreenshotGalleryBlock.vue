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
        <div class="shot-image-wrap">
          <img :src="item.image" :alt="item.title" loading="lazy" />
          <div class="shot-overlay">
            <span>{{ item.title }}</span>
          </div>
        </div>
        <div class="shot-info">
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
  gap: var(--space-m);
}

.gallery-head {
  display: grid;
  gap: 0.35rem;
}

.gallery-head h3,
.gallery-head p,
.shot-card h4,
.shot-card p {
  margin: 0;
}

.gallery-head h3 {
  font-size: var(--font-xl);
}

.gallery-head p {
  color: var(--muted);
  font-size: var(--font-body);
  line-height: 1.65;
}

.gallery-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(min(100%, 260px), 1fr));
  gap: var(--space-m);
}

.shot-card {
  display: grid;
  gap: 0.75rem;
  min-width: 0;
  padding: 0.65rem;
  border: 1px solid var(--line);
  border-radius: var(--radius-md);
  background: var(--surface);
  box-shadow: var(--shadow-soft);
  transition: transform var(--transition-normal), box-shadow var(--transition-normal);
}

.shot-card:hover {
  transform: translateY(-3px);
  box-shadow: var(--shadow-hover);
}

.shot-image-wrap {
  position: relative;
  overflow: hidden;
  border-radius: var(--radius-sm);
}

.shot-card img {
  display: block;
  width: 100%;
  aspect-ratio: 16 / 9;
  object-fit: cover;
  object-position: top center;
  background: #eef2f7;
  transition: transform var(--transition-slow);
}

.shot-card:hover .shot-image-wrap img {
  transform: scale(1.05);
}

.shot-overlay {
  position: absolute;
  inset: 0;
  display: flex;
  align-items: flex-end;
  justify-content: center;
  padding: 0.75rem;
  background: linear-gradient(to top, rgba(0, 0, 0, 0.5), transparent 60%);
  opacity: 0;
  transition: opacity var(--transition-normal);
}

.shot-card:hover .shot-overlay {
  opacity: 1;
}

.shot-overlay span {
  color: #fff;
  font-weight: 600;
  font-size: var(--font-sm);
}

.shot-info h4 {
  font-size: 0.95rem;
  overflow-wrap: anywhere;
}

.shot-info p {
  color: var(--muted);
  font-size: var(--font-sm);
  line-height: 1.5;
}

@media (max-width: 520px) {
  .gallery-head h3 {
    font-size: var(--font-lg);
  }
}
</style>
