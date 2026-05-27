<script setup lang="ts">
withDefaults(
  defineProps<{
    title?: string;
    content?: string;
    image?: string;
  }>(),
  {
    title: "图文区块",
    content: "",
    image: ""
  }
);
</script>

<template>
  <section class="panel">
    <div class="copy">
      <h3>{{ title }}</h3>
      <p>{{ content }}</p>
    </div>
    <div class="art">
      <img v-if="image" :src="image" :alt="title" />
      <div v-else class="placeholder">可接入发布后的静态资源</div>
    </div>
  </section>
</template>

<style scoped>
.panel {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
  gap: 1rem;
  padding: 1.5rem;
  border: 1px solid var(--line);
  border-radius: 24px;
  background: var(--surface);
}

.copy p {
  line-height: 1.8;
  color: var(--muted);
}

.art {
  display: grid;
  place-items: center;
  min-height: 220px;
  border-radius: 20px;
  background: linear-gradient(135deg, rgba(82, 134, 109, 0.22), rgba(178, 76, 42, 0.12));
}

.art img {
  max-width: 100%;
  border-radius: 20px;
}

.placeholder {
  padding: 1rem;
  text-align: center;
  color: var(--muted);
}
</style>
