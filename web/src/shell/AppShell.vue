<script setup lang="ts">
defineProps<{
  currentProductCode?: string;
  currentVersion?: string;
}>();

const emit = defineEmits<{
  home: [];
}>();
</script>

<template>
  <div class="shell">
    <header class="topbar">
      <button type="button" class="brand" @click="emit('home')">
        <strong>Update Platform</strong>
        <span>产品与插件发布</span>
      </button>

      <div class="topbar-meta">
        <span v-if="currentProductCode">{{ currentProductCode }}</span>
        <span v-if="currentVersion">v{{ currentVersion.replace(/^v/, "") }}</span>
      </div>
    </header>

    <main class="content">
      <slot />
    </main>
  </div>
</template>

<style scoped>
.shell {
  min-height: 100vh;
  padding: 1rem;
}

.topbar {
  display: flex;
  justify-content: space-between;
  gap: 1rem;
  align-items: center;
  max-width: 82rem;
  margin: 0 auto;
  padding: 0.8rem 1rem;
  border: 1px solid var(--line);
  border-radius: 8px;
  background: var(--surface);
  box-shadow: var(--shadow-soft);
}

.brand {
  display: inline-grid;
  gap: 0.12rem;
  padding: 0;
  border: 0;
  background: transparent;
  text-align: left;
  cursor: pointer;
}

.brand span {
  color: var(--muted);
  font-size: 0.78rem;
}

.brand strong {
  font-size: 1rem;
}

.topbar-meta {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-end;
  gap: 0.65rem;
}

.topbar-meta span {
  padding: 0.42rem 0.65rem;
  border: 1px solid var(--line);
  border-radius: 999px;
  background: var(--surface-subtle);
  color: var(--primary-dark);
  font-size: 0.84rem;
}

.content {
  max-width: 82rem;
  margin: 0 auto;
}

@media (max-width: 700px) {
  .shell {
    padding: 0.75rem;
  }

  .topbar {
    flex-direction: column;
    align-items: flex-start;
  }

  .topbar-meta {
    justify-content: flex-start;
  }
}
</style>
