<script setup lang="ts">
import type { SiteProfile } from "../runtime/api";

defineProps<{
  currentProductCode?: string;
  currentVersion?: string;
  profile?: SiteProfile;
}>();

const emit = defineEmits<{
  home: [];
}>();
</script>

<template>
  <div class="shell">
    <header class="topbar">
      <button type="button" class="brand" @click="emit('home')">
        <span class="brand-mark" aria-hidden="true">
          <span></span>
        </span>
        <span class="brand-copy">
          <strong>YXX Works</strong>
          <span>{{ profile?.subtitle || "产品、插件与创作实验" }}</span>
        </span>
      </button>

      <nav class="topbar-nav" aria-label="主导航">
        <a class="active" href="/#works">作品</a>
        <a v-if="profile?.github_url" :href="profile.github_url" target="_blank" rel="noreferrer">
          GitHub ↗
        </a>
      </nav>
    </header>

    <div v-if="currentProductCode || currentVersion" class="release-context">
      <span v-if="currentProductCode">{{ currentProductCode }}</span>
      <span v-if="currentVersion">v{{ currentVersion.replace(/^v/, "") }}</span>
    </div>

    <main class="content">
      <slot />
    </main>
  </div>
</template>

<style scoped>
.shell {
  min-height: 100vh;
  padding: 1rem 1.25rem 1.5rem;
}

.topbar {
  display: flex;
  justify-content: space-between;
  gap: 1rem;
  align-items: center;
  max-width: 78rem;
  margin: 0 auto;
  padding: 0.85rem 1.6rem;
  border: 1px solid rgba(224, 229, 240, 0.92);
  border-radius: 8px;
  background: rgba(255, 255, 255, 0.86);
  box-shadow: var(--shadow-soft);
  backdrop-filter: blur(18px);
}

.brand {
  display: inline-flex;
  gap: 0.85rem;
  align-items: center;
  padding: 0;
  border: 0;
  background: transparent;
  text-align: left;
  cursor: pointer;
}

.brand-mark {
  position: relative;
  display: grid;
  place-items: center;
  width: 2.25rem;
  height: 2.25rem;
  border-radius: 8px;
  background: linear-gradient(135deg, #7d5cff, #2e5cf4);
  box-shadow: 0 0.9rem 1.7rem rgba(84, 71, 228, 0.25);
}

.brand-mark span,
.brand-mark::before,
.brand-mark::after {
  position: absolute;
  width: 0.45rem;
  height: 1.45rem;
  border-radius: 999px;
  background: rgba(255, 255, 255, 0.94);
  content: "";
}

.brand-mark span {
  transform: translateY(0.28rem);
}

.brand-mark::before {
  transform: translate(-0.42rem, -0.33rem) rotate(-42deg);
}

.brand-mark::after {
  transform: translate(0.42rem, -0.33rem) rotate(42deg);
}

.brand-copy {
  display: grid;
  gap: 0.12rem;
}

.brand-copy span {
  color: var(--muted);
  font-size: 0.78rem;
}

.brand-copy strong {
  color: #121723;
  font-size: 1.08rem;
}

.topbar-nav {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-end;
  gap: 2.1rem;
  align-items: center;
}

.topbar-nav a {
  position: relative;
  color: #273044;
  font-size: 0.94rem;
  font-weight: 700;
}

.topbar-nav a.active {
  color: var(--primary);
}

.topbar-nav a.active::after {
  position: absolute;
  right: 0;
  bottom: -1.05rem;
  left: 0;
  height: 0.18rem;
  border-radius: 999px;
  background: var(--primary);
  content: "";
}

.release-context {
  display: flex;
  justify-content: flex-end;
  gap: 0.65rem;
  max-width: 78rem;
  margin: 0.75rem auto 0;
}

.release-context span {
  padding: 0.42rem 0.65rem;
  border: 1px solid rgba(218, 224, 236, 0.94);
  border-radius: 8px;
  background: rgba(255, 255, 255, 0.84);
  color: var(--primary-dark);
  font-size: 0.84rem;
}

.content {
  max-width: 78rem;
  margin: 0 auto;
}

@media (max-width: 700px) {
  .shell {
    padding: 0.75rem;
  }

  .topbar {
    flex-direction: column;
    align-items: flex-start;
    padding: 1rem;
  }

  .topbar-nav,
  .release-context {
    justify-content: flex-start;
  }

  .topbar-nav {
    gap: 1.2rem;
  }
}
</style>
