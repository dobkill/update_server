<script setup lang="ts">
import { ref } from "vue";
import type { SiteProfile } from "../runtime/api";

defineProps<{
  currentProductCode?: string;
  currentVersion?: string;
  profile?: SiteProfile;
}>();

const emit = defineEmits<{
  home: [];
}>();

const mobileMenuOpen = ref(false);

function toggleMenu() {
  mobileMenuOpen.value = !mobileMenuOpen.value;
}

function closeMenu() {
  mobileMenuOpen.value = false;
}
</script>

<template>
  <div class="shell">
    <header class="topbar-wrapper">
      <div class="topbar">
        <button type="button" class="brand" @click="emit('home'); closeMenu()">
          <span class="brand-mark" aria-hidden="true">
            <span></span>
          </span>
          <span class="brand-copy">
            <strong>{{ profile?.site_name || "YXX Works" }}</strong>
            <span>{{ profile?.subtitle || "产品、插件与创作实验" }}</span>
          </span>
        </button>

        <nav class="topbar-nav desktop-nav" aria-label="主导航">
          <a class="active" href="/#works">作品</a>
          <a v-if="profile?.github_url" :href="profile.github_url" target="_blank" rel="noreferrer">
            GitHub ↗
          </a>
        </nav>

        <button
          type="button"
          class="hamburger"
          :class="{ open: mobileMenuOpen }"
          aria-label="菜单"
          @click="toggleMenu"
        >
          <span></span>
          <span></span>
          <span></span>
        </button>
      </div>

      <!-- Mobile dropdown menu -->
      <Transition name="slide-down">
        <nav v-if="mobileMenuOpen" class="mobile-nav" aria-label="移动导航">
          <a class="active" href="/#works" @click="closeMenu">作品</a>
          <a v-if="profile?.github_url" :href="profile.github_url" target="_blank" rel="noreferrer" @click="closeMenu">
            GitHub ↗
          </a>
        </nav>
      </Transition>
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
  width: 100%;
  min-height: 100vh;
  padding: var(--space-s) var(--space-m) var(--space-xl);
}

/* ─── Topbar Wrapper (sticky) ─── */
.topbar-wrapper {
  position: sticky;
  top: 0;
  z-index: 100;
  width: min(100%, 78rem);
  max-width: 78rem;
  margin: 0 auto;
}

/* ─── Topbar ─── */
.topbar {
  display: flex;
  justify-content: space-between;
  gap: var(--space-m);
  align-items: center;
  padding: 0.75rem clamp(0.75rem, 2vw, 1.6rem);
  border: 1px solid rgba(224, 229, 240, 0.92);
  border-radius: var(--radius-lg);
  background: rgba(255, 255, 255, 0.88);
  box-shadow: var(--shadow-soft);
  backdrop-filter: blur(18px);
  -webkit-backdrop-filter: blur(18px);
}

/* ─── Brand ─── */
.brand {
  display: inline-flex;
  gap: 0.75rem;
  align-items: center;
  min-width: 0;
  padding: 0;
  border: 0;
  background: transparent;
  text-align: left;
}

.brand:hover {
  color: inherit;
}

.brand-mark {
  position: relative;
  display: grid;
  place-items: center;
  width: 2.25rem;
  height: 2.25rem;
  flex-shrink: 0;
  border-radius: var(--radius-sm);
  background: linear-gradient(135deg, #7d5cff, #2e5cf4);
  box-shadow: 0 0.6rem 1.2rem rgba(84, 71, 228, 0.25);
  transition: transform var(--transition-fast), box-shadow var(--transition-fast);
}

.brand:hover .brand-mark {
  transform: scale(1.05);
  box-shadow: 0 0.8rem 1.6rem rgba(84, 71, 228, 0.35);
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
  gap: 0.1rem;
  min-width: 0;
}

.brand-copy span {
  color: var(--muted);
  font-size: var(--font-sm);
}

.brand-copy strong {
  color: #121723;
  font-size: clamp(0.95rem, 1.2vw, 1.08rem);
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

/* ─── Desktop Nav ─── */
.desktop-nav {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-end;
  gap: clamp(1rem, 2vw, 2.1rem);
  align-items: center;
  min-width: 0;
}

.topbar-nav a {
  position: relative;
  color: #273044;
  font-size: clamp(0.85rem, 1vw, 0.94rem);
  font-weight: 700;
  padding: 0.25rem 0;
  transition: color var(--transition-fast);
}

.topbar-nav a:hover {
  color: var(--primary);
}

.topbar-nav a.active {
  color: var(--primary);
}

.topbar-nav a.active::after {
  position: absolute;
  right: 0;
  bottom: -2px;
  left: 0;
  height: 2.5px;
  border-radius: 999px;
  background: var(--primary);
  content: "";
  transform: scaleX(1);
  transition: transform var(--transition-normal);
}

/* ─── Hamburger Button (hidden on desktop) ─── */
.hamburger {
  display: none;
  flex-direction: column;
  justify-content: center;
  gap: 5px;
  width: 2.25rem;
  height: 2.25rem;
  padding: 0;
  border: 1px solid var(--line);
  border-radius: var(--radius-sm);
  background: transparent;
  flex-shrink: 0;
}

.hamburger:hover {
  background: var(--surface-subtle);
  border-color: var(--primary-light);
}

.hamburger span {
  display: block;
  width: 14px;
  height: 2px;
  margin: 0 auto;
  border-radius: 999px;
  background: #273044;
  transition: all var(--transition-fast);
  transform-origin: center;
}

.hamburger.open span:nth-child(1) {
  transform: translateY(7px) rotate(45deg);
}

.hamburger.open span:nth-child(2) {
  opacity: 0;
}

.hamburger.open span:nth-child(3) {
  transform: translateY(-7px) rotate(-45deg);
}

/* ─── Mobile Nav (hidden on desktop) ─── */
.mobile-nav {
  display: none;
  flex-direction: column;
  gap: 0;
  margin-top: 0.5rem;
  padding: 0.5rem;
  border: 1px solid rgba(224, 229, 240, 0.92);
  border-radius: var(--radius-md);
  background: rgba(255, 255, 255, 0.95);
  box-shadow: var(--shadow-soft);
  backdrop-filter: blur(18px);
}

.mobile-nav a {
  display: block;
  padding: 0.7rem 1rem;
  border-radius: var(--radius-sm);
  color: #273044;
  font-weight: 600;
  font-size: 0.95rem;
  transition: background var(--transition-fast), color var(--transition-fast);
}

.mobile-nav a:hover {
  background: var(--surface-subtle);
  color: var(--primary);
}

.mobile-nav a.active {
  color: var(--primary);
  background: rgba(79, 70, 229, 0.06);
}

/* ─── Slide-down transition ─── */
.slide-down-enter-active,
.slide-down-leave-active {
  transition: all var(--transition-normal);
  overflow: hidden;
}

.slide-down-enter-from,
.slide-down-leave-to {
  opacity: 0;
  transform: translateY(-8px);
  max-height: 0;
  margin-top: 0;
}

.slide-down-enter-to,
.slide-down-leave-from {
  opacity: 1;
  transform: translateY(0);
  max-height: 200px;
}

/* ─── Release Context ─── */
.release-context {
  display: flex;
  flex-wrap: wrap;
  justify-content: flex-end;
  gap: var(--space-xs);
  width: min(100%, 78rem);
  max-width: 78rem;
  margin: 0.5rem auto 0;
}

.release-context span {
  padding: 0.35rem 0.6rem;
  border: 1px solid rgba(218, 224, 236, 0.94);
  border-radius: var(--radius-sm);
  background: rgba(255, 255, 255, 0.84);
  color: var(--primary-dark);
  font-size: var(--font-sm);
}

/* ─── Content ─── */
.content {
  width: min(100%, 78rem);
  max-width: 78rem;
  margin: 0 auto;
}

/* ─── Responsive ─── */
@media (max-width: 700px) {
  .shell {
    padding: var(--space-xs) var(--space-s) var(--space-l);
  }

  .desktop-nav {
    display: none;
  }

  .hamburger {
    display: flex;
  }

  .mobile-nav {
    display: flex;
  }

  .topbar {
    padding: 0.65rem 0.85rem;
  }

  .brand-copy strong,
  .brand-copy span {
    overflow-wrap: anywhere;
  }

  .release-context {
    justify-content: flex-start;
  }
}
</style>
