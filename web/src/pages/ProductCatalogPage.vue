<script setup lang="ts">
import { computed } from "vue";
import type {
  FutureDirection,
  PortfolioHomeData,
  PortfolioProduct,
  RecentUpdate
} from "../runtime/api";

type PlaceholderWork = {
  title: string;
  status: string;
  summary: string;
  icon: string;
};

const props = defineProps<{
  home: PortfolioHomeData;
}>();

const emit = defineEmits<{
  open: [productCode: string];
}>();

const placeholderWorks: PlaceholderWork[] = [
  {
    title: "新作品即将加入",
    status: "准备中",
    summary: "更多有趣的插件与工具正在构思与开发中。",
    icon: "+"
  },
  {
    title: "更多插件计划中",
    status: "计划中",
    summary: "围绕知识管理与效率提升，持续探索新的想法。",
    icon: "✦"
  },
  {
    title: "工具类作品准备中",
    status: "准备中",
    summary: "轻量、好用的效率工具，正在打磨中。",
    icon: "⌁"
  }
];

const fallbackDirections: FutureDirection[] = [
  {
    title: "Obsidian 插件增强",
    comment: "持续优化记录、任务与知识管理体验。",
    icon_path: ""
  },
  {
    title: "效率工具",
    comment: "开发更轻量、专注的个人效率工具。",
    icon_path: ""
  },
  {
    title: "Web 实验",
    comment: "尝试小型 Web 产品与交互实验，探索新的表达方式。",
    icon_path: ""
  }
];

const primaryWork = computed<PortfolioProduct | null>(
  () => props.home.recommendations[0] ?? props.home.products[0] ?? null
);

const products = computed(() => props.home.products);

const recentUpdates = computed<RecentUpdate[]>(() => {
  const realUpdates = props.home.recent_updates.filter((item) => item.title && item.description);
  return [
    ...realUpdates,
    {
      product_code: "",
      name: "",
      version: "",
      published_at: "",
      title: "后续更新",
      description: "将继续补充插件体验优化、新工具实验和版本记录。"
    }
  ];
});

const latestUpdate = computed(() => props.home.recent_updates[0] ?? null);
const futureDirections = computed(() =>
  props.home.future_directions.length ? props.home.future_directions : fallbackDirections
);

function normalizeVersion(version?: string): string {
  if (!version) {
    return "";
  }
  return version.startsWith("v") ? version : `v${version}`;
}

function formatDate(value?: string): string {
  if (!value) {
    return "";
  }
  return value.slice(0, 10);
}

function productMeta(product: PortfolioProduct): string {
  const version = normalizeVersion(product.latest_version);
  const date = formatDate(product.published_at || product.updated_at);
  return [version, date].filter(Boolean).join(" · ");
}

function updateMeta(update: RecentUpdate | null, product: PortfolioProduct): string {
  if (update) {
    return [normalizeVersion(update.version), formatDate(update.published_at)].filter(Boolean).join(" · ");
  }
  return productMeta(product);
}

function directionIcon(index: number): string {
  return ["◇", "◷", "</>"][index] ?? "◇";
}

function openWork(product: PortfolioProduct): void {
  if (product.product_code) {
    emit("open", product.product_code);
  }
}
</script>

<template>
  <section class="portfolio-hero">
    <div class="hero-copy">
      <h1>个人作品集</h1>
      <p>设计、开发与持续迭代中的作品。</p>
      <div class="hero-actions" aria-label="首页操作">
        <a class="primary-action" href="#works">浏览作品 <span aria-hidden="true">→</span></a>
        <a
          v-if="home.profile.github_url"
          class="secondary-action"
          :href="home.profile.github_url"
          target="_blank"
          rel="noreferrer"
        >
          查看 GitHub <span aria-hidden="true">↗</span>
        </a>
      </div>
    </div>

    <div v-if="primaryWork" class="hero-feature" aria-label="主推项目">
      <div class="ghost-card one"></div>
      <div class="ghost-card two"></div>
      <article class="spotlight-card">
        <div class="cover-frame large">
          <img
            v-if="primaryWork.cover_image_url"
            :src="primaryWork.cover_image_url"
            :alt="primaryWork.name"
          />
          <div v-else class="daily-mark" aria-hidden="true">
            <span></span>
            <strong>Daily</strong>
          </div>
        </div>
        <div class="spotlight-copy">
          <span class="eyebrow">主推项目</span>
          <h2>{{ primaryWork.name }}</h2>
          <div class="chip-row">
            <span>{{ primaryWork.tag || "Obsidian 插件" }}</span>
            <span>{{ primaryWork.status_label || "已发布" }}</span>
          </div>
          <p>{{ primaryWork.summary }}</p>
        </div>
      </article>
    </div>
  </section>

  <section v-if="primaryWork" class="portfolio-section">
    <header class="section-heading">
      <h2>最近更新</h2>
    </header>

    <div class="updates-panel">
      <article class="update-card">
        <div class="cover-frame">
          <img
            v-if="primaryWork.cover_image_url"
            :src="primaryWork.cover_image_url"
            :alt="primaryWork.name"
          />
          <div v-else class="daily-mark compact" aria-hidden="true">
            <span></span>
            <strong>Daily</strong>
          </div>
        </div>
        <div class="update-copy">
          <div class="chip-row">
            <span>{{ primaryWork.status_label || "已发布" }}</span>
            <span>{{ primaryWork.tag || "Obsidian 插件" }}</span>
          </div>
          <h3>{{ primaryWork.name }}</h3>
          <p>{{ primaryWork.summary }}</p>
          <span class="meta-line">{{ updateMeta(latestUpdate, primaryWork) }}</span>
          <div class="card-actions">
            <button type="button" class="text-action" @click="openWork(primaryWork)">
              查看详情 <span aria-hidden="true">→</span>
            </button>
            <a
              v-if="primaryWork.github_url"
              class="text-action"
              :href="primaryWork.github_url"
              target="_blank"
              rel="noreferrer"
            >
              GitHub <span aria-hidden="true">↗</span>
            </a>
          </div>
        </div>
      </article>

      <div class="timeline" aria-label="更新时间线">
        <article
          v-for="(item, index) in recentUpdates"
          :key="`${item.title}-${index}`"
          class="timeline-item"
          :class="{ active: index === 0 }"
        >
          <span class="timeline-dot" aria-hidden="true"></span>
          <div>
            <h3>{{ item.title }}</h3>
            <p>{{ item.description }}</p>
          </div>
        </article>
      </div>
    </div>
  </section>

  <section class="portfolio-section" id="works">
    <header class="section-heading">
      <h2>全部作品</h2>
    </header>

    <div class="work-grid">
      <article v-for="product in products" :key="product.product_code" class="work-card published">
        <div class="cover-frame small">
          <img
            v-if="product.cover_image_url"
            :src="product.cover_image_url"
            :alt="product.name"
          />
          <div v-else class="daily-mark compact" aria-hidden="true">
            <span></span>
            <strong>Daily</strong>
          </div>
        </div>
        <div class="work-copy">
          <div class="chip-row">
            <span>{{ product.status_label || "已发布" }}</span>
            <span>{{ product.tag || "Obsidian 插件" }}</span>
          </div>
          <h3>{{ product.name }}</h3>
          <p>{{ product.summary }}</p>
          <span class="meta-line">{{ productMeta(product) }}</span>
          <div class="card-actions">
            <button type="button" class="text-action" @click="openWork(product)">
              查看详情 <span aria-hidden="true">→</span>
            </button>
            <a
              v-if="product.github_url"
              class="text-action"
              :href="product.github_url"
              target="_blank"
              rel="noreferrer"
            >
              GitHub <span aria-hidden="true">↗</span>
            </a>
          </div>
        </div>
      </article>

      <article v-for="item in placeholderWorks" :key="item.title" class="work-card placeholder">
        <div class="placeholder-icon" aria-hidden="true">{{ item.icon }}</div>
        <div class="work-copy">
          <div class="chip-row muted-chip">
            <span>{{ item.status }}</span>
          </div>
          <h3>{{ item.title }}</h3>
          <p>{{ item.summary }}</p>
        </div>
      </article>
    </div>
  </section>

  <section class="portfolio-section">
    <header class="section-heading">
      <h2>未来方向</h2>
    </header>

    <div class="direction-grid">
      <article v-for="(item, index) in futureDirections" :key="item.title" class="direction-card">
        <span class="direction-icon" aria-hidden="true">{{ directionIcon(index) }}</span>
        <h3>{{ item.title }}</h3>
        <p>{{ item.comment }}</p>
      </article>
    </div>
  </section>

  <footer class="portfolio-footer">
    <div class="footer-brand">
      <span class="footer-logo" aria-hidden="true">
        <span></span>
      </span>
      <span>© {{ new Date().getFullYear() }} {{ home.profile.site_name || "YXX Works" }}</span>
    </div>
    <nav v-if="home.profile.github_url || home.profile.email" aria-label="站点链接" class="footer-nav">
      <a v-if="home.profile.github_url" :href="home.profile.github_url" target="_blank" rel="noreferrer">
        GitHub
      </a>
      <a v-if="home.profile.email" :href="`mailto:${home.profile.email}`">邮件联系</a>
    </nav>
  </footer>
</template>

<style scoped>
/* ═══════════════════════════════════════
   Hero Section
   ═══════════════════════════════════════ */
.portfolio-hero {
  display: grid;
  grid-template-columns: minmax(0, 1fr) minmax(0, 1fr);
  gap: clamp(1.5rem, 4vw, 3.2rem);
  align-items: center;
  min-height: auto;
  padding: clamp(1.5rem, 4vw, 3rem) clamp(1rem, 3vw, 2.8rem) clamp(1rem, 2vw, 2.2rem);
}

.hero-copy {
  display: grid;
  gap: clamp(0.75rem, 1.5vw, 1.35rem);
  min-width: 0;
  max-width: 40rem;
}

.hero-copy h1 {
  margin: 0;
  color: #0f1422;
  font-size: clamp(1.8rem, 5vw, 2.75rem);
  line-height: 1.12;
  letter-spacing: -0.02em;
}

.hero-copy p {
  max-width: 34rem;
  margin: 0;
  color: #536072;
  font-size: var(--font-body);
  line-height: 1.8;
}

.hero-actions,
.card-actions,
.chip-row {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-s);
  align-items: center;
}

.primary-action,
.secondary-action,
.text-action {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: 0.35rem;
  max-width: 100%;
  min-height: 2.6rem;
  border-radius: var(--radius-md);
  text-align: center;
  font-weight: 700;
  transition: all var(--transition-normal);
}

.primary-action,
.secondary-action {
  padding: 0.65rem 1.2rem;
}

.primary-action {
  border: 1px solid #4f46e5;
  background: linear-gradient(135deg, #4f46e5, #2466d8);
  color: #fff;
  box-shadow: 0 8px 20px rgba(55, 79, 211, 0.22);
}

.primary-action:hover {
  transform: translateY(-1px);
  box-shadow: 0 12px 28px rgba(55, 79, 211, 0.32);
}

.primary-action:active {
  transform: translateY(0);
}

.secondary-action {
  border: 1px solid #dce3ef;
  background: rgba(255, 255, 255, 0.84);
  color: #182236;
}

.secondary-action:hover {
  border-color: var(--primary-light);
  background: #fff;
  color: var(--primary);
}

/* ─── Hero Feature ─── */
.hero-feature {
  position: relative;
  min-width: 0;
}

.ghost-card,
.spotlight-card,
.updates-panel,
.work-card,
.direction-card {
  border: 1px solid rgba(219, 225, 238, 0.94);
  border-radius: var(--radius-md);
  background: rgba(255, 255, 255, 0.9);
  box-shadow: var(--shadow-soft);
}

.ghost-card {
  position: absolute;
  right: 1rem;
  width: 82%;
  height: 72%;
  opacity: 0.56;
  pointer-events: none;
}

.ghost-card.one {
  top: 0.65rem;
  transform: rotate(4deg);
}

.ghost-card.two {
  top: 2.1rem;
  right: 2.1rem;
  transform: rotate(-5deg);
}

.spotlight-card {
  position: relative;
  display: grid;
  grid-template-columns: 0.9fr 1fr;
  gap: clamp(0.75rem, 1.5vw, 1.2rem);
  min-width: 0;
  padding: clamp(0.75rem, 1.5vw, 1.15rem);
  transition: box-shadow var(--transition-normal), transform var(--transition-normal);
}

.spotlight-card:hover {
  box-shadow: var(--shadow-hover);
}

/* ─── Cover Frames ─── */
.cover-frame {
  position: relative;
  display: grid;
  place-items: center;
  overflow: hidden;
  border-radius: var(--radius-sm);
  background:
    linear-gradient(135deg, rgba(255, 255, 255, 0.26), rgba(255, 255, 255, 0)),
    linear-gradient(135deg, #11145c, #2932b5 48%, #0f9f8f);
}

.cover-frame.large {
  aspect-ratio: 4 / 3;
}

.cover-frame.small,
.update-card .cover-frame {
  width: 100%;
  aspect-ratio: 4 / 3;
  max-width: 10rem;
  flex: 0 0 auto;
}

.cover-frame img {
  width: 100%;
  height: 100%;
  object-fit: cover;
  transition: transform var(--transition-slow);
}

.spotlight-card:hover .cover-frame img,
.work-card:hover .cover-frame img {
  transform: scale(1.03);
}

/* ─── Daily Mark (placeholder) ─── */
.daily-mark {
  position: relative;
  display: grid;
  gap: 0.68rem;
  place-items: center;
  width: min(78%, 13rem);
  aspect-ratio: 1.16;
  padding: 1rem;
  border: 1px solid rgba(255, 255, 255, 0.36);
  border-radius: var(--radius-sm);
  background: rgba(255, 255, 255, 0.13);
  color: #fff;
  box-shadow: inset 0 0 2rem rgba(255, 255, 255, 0.14);
}

.daily-mark.compact {
  width: clamp(4rem, 8vw, 6.5rem);
}

.daily-mark span {
  width: 58%;
  height: 0.56rem;
  border-radius: 999px;
  background: rgba(255, 255, 255, 0.78);
  box-shadow:
    0 1.2rem 0 rgba(255, 255, 255, 0.45),
    0 2.4rem 0 rgba(255, 255, 255, 0.28);
}

.daily-mark strong {
  letter-spacing: 0;
}

/* ─── Shared Typography ─── */
.spotlight-copy,
.update-copy,
.work-copy,
.direction-card {
  display: grid;
  align-content: center;
  gap: 0.65rem;
  min-width: 0;
}

.eyebrow,
.chip-row span {
  width: fit-content;
  border: 1px solid #dfe6f2;
  border-radius: 999px;
  background: #f7f9ff;
  color: #405071;
  font-size: clamp(0.7rem, 0.9vw, 0.78rem);
  font-weight: 700;
  letter-spacing: 0.02em;
}

.eyebrow {
  padding: 0.28rem 0.65rem;
}

.chip-row span {
  padding: 0.22rem 0.55rem;
  max-width: 100%;
  overflow-wrap: anywhere;
}

.muted-chip span {
  background: #f3f5f9;
  color: #687386;
}

.spotlight-card h2,
.update-card h3,
.work-card h3,
.direction-card h3,
.timeline-item h3 {
  margin: 0;
  color: #111827;
  line-height: 1.25;
}

.spotlight-card h2 {
  font-size: var(--font-xl);
}

.spotlight-card p,
.update-card p,
.work-card p,
.direction-card p,
.timeline-item p {
  margin: 0;
  color: #5d687a;
  font-size: var(--font-sm);
  line-height: 1.72;
  overflow-wrap: anywhere;
}

/* ═══════════════════════════════════════
   Sections
   ═══════════════════════════════════════ */
.portfolio-section {
  margin-top: var(--space-xl);
}

.section-heading {
  margin-bottom: var(--space-m);
}

.section-heading h2 {
  margin: 0;
  font-size: var(--font-xl);
  line-height: 1.2;
  letter-spacing: -0.01em;
}

/* ═══════════════════════════════════════
   Updates Panel
   ═══════════════════════════════════════ */
.updates-panel {
  display: grid;
  grid-template-columns: minmax(0, 1fr) minmax(0, 1fr);
  gap: clamp(0.75rem, 1.5vw, 1.3rem);
  padding: clamp(0.75rem, 1.5vw, 1.25rem);
}

.update-card {
  display: flex;
  gap: var(--space-m);
  align-items: center;
  min-width: 0;
}

.meta-line {
  color: #6b7588;
  font-size: var(--font-sm);
}

.text-action {
  min-height: auto;
  padding: 0;
  border: 0;
  background: transparent;
  color: var(--primary);
  cursor: pointer;
  font-size: var(--font-sm);
}

.text-action:hover {
  color: var(--primary-dark);
}

/* ─── Timeline ─── */
.timeline {
  display: grid;
  gap: 0;
  padding: 0.1rem 0.2rem 0.1rem 0;
}

.timeline-item {
  position: relative;
  display: grid;
  grid-template-columns: 1.35rem minmax(0, 1fr);
  gap: 0.8rem;
  padding: 0 0 1.05rem;
}

.timeline-item::before {
  position: absolute;
  top: 1.05rem;
  bottom: 0;
  left: 0.44rem;
  width: 1.5px;
  background: #dce3ef;
  content: "";
}

.timeline-item:last-child::before {
  display: none;
}

.timeline-dot {
  position: relative;
  z-index: 1;
  width: 0.86rem;
  height: 0.86rem;
  margin-top: 0.2rem;
  border-radius: 50%;
  background: #c8d0de;
  transition: all var(--transition-normal);
}

.timeline-item.active .timeline-dot {
  background: var(--primary);
  box-shadow: 0 0 0 0.34rem rgba(79, 70, 229, 0.13);
}

.timeline-item h3 {
  font-size: clamp(0.85rem, 1vw, 0.98rem);
}

.timeline-item p {
  margin-top: 0.2rem;
  font-size: clamp(0.82rem, 0.95vw, 0.92rem);
}

/* ═══════════════════════════════════════
   Work Grid
   ═══════════════════════════════════════ */
.work-grid,
.direction-grid {
  display: grid;
  gap: var(--space-m);
}

.work-grid {
  grid-template-columns: repeat(auto-fill, minmax(min(100%, 16rem), 1fr));
}

.direction-grid {
  grid-template-columns: repeat(auto-fill, minmax(min(100%, 16rem), 1fr));
}

.work-card {
  display: grid;
  gap: var(--space-m);
  align-content: start;
  padding: clamp(0.75rem, 1.5vw, 1rem);
  transition: transform var(--transition-normal), box-shadow var(--transition-normal);
}

.work-card:hover {
  transform: translateY(-3px);
  box-shadow: var(--shadow-hover);
}

.placeholder {
  border-style: dashed;
  background: linear-gradient(
    135deg,
    rgba(250, 252, 255, 0.82),
    rgba(243, 246, 255, 0.6)
  );
}

.placeholder-icon,
.direction-icon {
  display: grid;
  place-items: center;
  width: 2.8rem;
  height: 2.8rem;
  border: 1px solid #dfe6f2;
  border-radius: var(--radius-sm);
  background: #f7f9ff;
  color: #39445c;
  font-weight: 900;
  font-size: 1.1rem;
  transition: all var(--transition-normal);
}

.direction-card {
  padding: clamp(0.75rem, 1.5vw, 1rem);
  transition: transform var(--transition-normal), box-shadow var(--transition-normal), border-color var(--transition-normal);
}

.direction-card:hover {
  transform: translateY(-2px);
  box-shadow: var(--shadow-hover);
  border-color: var(--primary-light);
}

.direction-card:hover .direction-icon {
  background: rgba(79, 70, 229, 0.08);
  border-color: var(--primary-light);
  color: var(--primary);
}

/* ═══════════════════════════════════════
   Footer
   ═══════════════════════════════════════ */
.portfolio-footer {
  display: flex;
  justify-content: space-between;
  gap: var(--space-m);
  align-items: center;
  margin: var(--space-2xl) 0 0;
  padding: var(--space-l) var(--space-m) var(--space-s);
  border-top: 1px solid #e2e7f1;
  color: #667187;
  font-size: var(--font-sm);
}

.footer-brand {
  display: flex;
  align-items: center;
  gap: var(--space-s);
}

.footer-logo {
  position: relative;
  display: grid;
  place-items: center;
  width: 1.5rem;
  height: 1.5rem;
  border-radius: 4px;
  background: linear-gradient(135deg, #7d5cff, #2e5cf4);
  flex-shrink: 0;
}

.footer-logo span {
  width: 0.3rem;
  height: 0.9rem;
  border-radius: 999px;
  background: rgba(255, 255, 255, 0.9);
}

.footer-nav {
  display: flex;
  flex-wrap: wrap;
  gap: var(--space-m);
  align-items: center;
}

.footer-nav a {
  color: #667187;
  transition: color var(--transition-fast);
}

.footer-nav a:hover {
  color: var(--primary);
}

/* ═══════════════════════════════════════
   Responsive Breakpoints
   ═══════════════════════════════════════ */
@media (max-width: 980px) {
  .portfolio-hero,
  .updates-panel {
    grid-template-columns: 1fr;
  }

  .portfolio-hero {
    gap: clamp(1.2rem, 3vw, 2rem);
    padding: clamp(1.5rem, 3vw, 2.4rem) clamp(0.75rem, 2vw, 1.5rem) clamp(1rem, 2vw, 1.6rem);
  }
}

@media (max-width: 680px) {
  .portfolio-hero {
    padding: clamp(1rem, 3vw, 2rem) 0 clamp(0.5rem, 2vw, 1rem);
  }

  .spotlight-card,
  .work-grid,
  .direction-grid {
    grid-template-columns: 1fr;
  }

  .hero-feature {
    min-height: auto;
  }

  .ghost-card {
    display: none;
  }

  .spotlight-card {
    padding: 0.75rem;
  }

  .cover-frame.large {
    min-height: auto;
  }

  .updates-panel {
    padding: 0.75rem;
  }

  .update-card {
    flex-direction: column;
    align-items: flex-start;
  }

  .update-card .cover-frame {
    width: 100%;
    max-width: none;
    aspect-ratio: 16 / 9;
    flex-basis: auto;
  }

  .work-card {
    padding: 0.85rem;
  }

  .portfolio-footer {
    flex-direction: column;
    align-items: flex-start;
    gap: var(--space-s);
  }
}

@media (max-width: 420px) {
  .hero-actions,
  .card-actions {
    align-items: stretch;
  }

  .primary-action,
  .secondary-action {
    width: 100%;
  }

  .cover-frame.small,
  .update-card .cover-frame {
    max-width: none;
    width: 100%;
    aspect-ratio: 16 / 9;
  }

  .work-grid,
  .direction-grid {
    grid-template-columns: 1fr;
  }
}
</style>
