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
    <div class="backdrop backdrop-a" />
    <div class="backdrop backdrop-b" />

    <header class="topbar">
      <button type="button" class="brand" @click="emit('home')">
        <span>Software Update Platform</span>
        <strong>Web Module</strong>
      </button>

      <div class="topbar-meta">
        <span v-if="currentProductCode">Product: {{ currentProductCode }}</span>
        <span v-if="currentVersion">Version: {{ currentVersion }}</span>
      </div>
    </header>

    <main class="content">
      <slot />
    </main>
  </div>
</template>

<style scoped>
.shell {
  position: relative;
  overflow: hidden;
  padding: 1.5rem;
}

.backdrop {
  position: fixed;
  inset: auto;
  pointer-events: none;
  border-radius: 999px;
  filter: blur(12px);
  opacity: 0.7;
}

.backdrop-a {
  top: -7rem;
  right: -4rem;
  width: 24rem;
  height: 24rem;
  background: radial-gradient(circle, rgba(207, 103, 54, 0.28), transparent 68%);
}

.backdrop-b {
  left: -6rem;
  bottom: 8rem;
  width: 22rem;
  height: 22rem;
  background: radial-gradient(circle, rgba(53, 126, 102, 0.18), transparent 68%);
}

.topbar {
  display: flex;
  justify-content: space-between;
  gap: 1rem;
  align-items: center;
  max-width: 80rem;
  margin: 0 auto;
  padding: 1.1rem 1.35rem;
  border: 1px solid var(--line);
  border-radius: 999px;
  background:
    linear-gradient(135deg, rgba(255, 248, 237, 0.96), rgba(246, 239, 221, 0.78)),
    var(--surface);
  box-shadow: var(--shadow);
  position: relative;
  z-index: 1;
}

.brand {
  display: grid;
  gap: 0.15rem;
  padding: 0;
  border: 0;
  background: transparent;
  text-align: left;
  cursor: pointer;
}

.brand span {
  color: var(--muted);
  font-size: 0.76rem;
  letter-spacing: 0.12em;
  text-transform: uppercase;
}

.brand strong {
  font-size: 1.1rem;
}

.topbar-meta {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-end;
  gap: 0.65rem;
}

.topbar-meta span {
  padding: 0.55rem 0.85rem;
  border-radius: 999px;
  background: rgba(31, 31, 26, 0.9);
  color: #fff7ef;
  font-size: 0.84rem;
}

.content {
  max-width: 80rem;
  margin: 0 auto;
  position: relative;
  z-index: 1;
}

@media (max-width: 700px) {
  .shell {
    padding: 1rem;
  }

  .topbar {
    flex-direction: column;
    align-items: flex-start;
    border-radius: 28px;
  }

  .topbar-meta {
    justify-content: flex-start;
  }
}
</style>
