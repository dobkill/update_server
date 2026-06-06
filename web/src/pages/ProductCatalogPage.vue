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
        <a class="primary-action" href="#works">浏览作品 <span aria-hidden="true">-&gt;</span></a>
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
              查看详情 <span aria-hidden="true">-&gt;</span>
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
              查看详情 <span aria-hidden="true">-&gt;</span>
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
    <span>© 2026 YXX Works · 个人作品集</span>
    <nav v-if="home.profile.github_url || home.profile.email" aria-label="站点链接">
      <a v-if="home.profile.github_url" :href="home.profile.github_url" target="_blank" rel="noreferrer">
        GitHub
      </a>
      <a v-if="home.profile.email" :href="`mailto:${home.profile.email}`">邮件</a>
    </nav>
  </footer>
</template>

<style scoped>
.portfolio-hero {
  display: grid;
  grid-template-columns: minmax(0, 0.95fr) minmax(20rem, 0.88fr);
  gap: 3.2rem;
  align-items: center;
  min-height: 23rem;
  padding: 3rem 2.8rem 2.2rem;
}

.hero-copy {
  display: grid;
  gap: 1.35rem;
  min-width: 0;
  max-width: 40rem;
}

.hero-copy h1 {
  margin: 0;
  color: #0f1422;
  font-size: 2.75rem;
  line-height: 1.12;
}

.hero-copy p {
  max-width: 34rem;
  margin: 0;
  color: #536072;
  font-size: 1.08rem;
  line-height: 1.9;
}

.hero-actions,
.card-actions,
.chip-row {
  display: flex;
  flex-wrap: wrap;
  gap: 0.75rem;
  align-items: center;
}

.primary-action,
.secondary-action,
.text-action {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  max-width: 100%;
  min-height: 2.75rem;
  border-radius: 8px;
  text-align: center;
  font-weight: 700;
}

.primary-action,
.secondary-action {
  padding: 0.7rem 1.3rem;
}

.primary-action {
  border: 1px solid #4f46e5;
  background: linear-gradient(135deg, #4f46e5, #2466d8);
  color: #fff;
  box-shadow: 0 14px 26px rgba(55, 79, 211, 0.22);
}

.secondary-action {
  border: 1px solid #dce3ef;
  background: rgba(255, 255, 255, 0.84);
  color: #182236;
}

.hero-feature {
  position: relative;
  min-width: 0;
  min-height: 20rem;
}

.ghost-card,
.spotlight-card,
.updates-panel,
.work-card,
.direction-card {
  border: 1px solid rgba(219, 225, 238, 0.94);
  border-radius: 8px;
  background: rgba(255, 255, 255, 0.9);
  box-shadow: var(--shadow-soft);
}

.ghost-card {
  position: absolute;
  right: 1rem;
  width: 82%;
  height: 72%;
  opacity: 0.56;
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
  gap: 1.2rem;
  min-width: 0;
  min-height: 18rem;
  padding: 1.15rem;
}

.cover-frame {
  position: relative;
  display: grid;
  place-items: center;
  overflow: hidden;
  border-radius: 8px;
  background:
    linear-gradient(135deg, rgba(255, 255, 255, 0.26), rgba(255, 255, 255, 0)),
    linear-gradient(135deg, #11145c, #2932b5 48%, #0f9f8f);
}

.cover-frame.large {
  aspect-ratio: 4 / 3;
  min-height: 15.5rem;
}

.cover-frame.small,
.update-card .cover-frame {
  width: 8.7rem;
  height: 6.4rem;
  flex: 0 0 8.7rem;
}

.cover-frame img {
  width: 100%;
  height: 100%;
  object-fit: cover;
}

.daily-mark {
  position: relative;
  display: grid;
  gap: 0.68rem;
  place-items: center;
  width: min(78%, 13rem);
  aspect-ratio: 1.16;
  padding: 1rem;
  border: 1px solid rgba(255, 255, 255, 0.36);
  border-radius: 8px;
  background: rgba(255, 255, 255, 0.13);
  color: #fff;
  box-shadow: inset 0 0 2rem rgba(255, 255, 255, 0.14);
}

.daily-mark.compact {
  width: 6.5rem;
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

.spotlight-copy,
.update-copy,
.work-copy,
.direction-card {
  display: grid;
  align-content: center;
  gap: 0.78rem;
  min-width: 0;
}

.eyebrow,
.chip-row span {
  width: fit-content;
  border: 1px solid #dfe6f2;
  border-radius: 8px;
  background: #f7f9ff;
  color: #405071;
  font-size: 0.78rem;
  font-weight: 800;
}

.eyebrow {
  padding: 0.32rem 0.58rem;
}

.chip-row span {
  padding: 0.28rem 0.54rem;
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
  font-size: 1.65rem;
}

.spotlight-card p,
.update-card p,
.work-card p,
.direction-card p,
.timeline-item p {
  margin: 0;
  color: #5d687a;
  line-height: 1.72;
  overflow-wrap: anywhere;
}

.portfolio-section {
  margin-top: 1.5rem;
}

.section-heading {
  margin-bottom: 0.8rem;
}

.section-heading h2 {
  margin: 0;
  font-size: 1.22rem;
  line-height: 1.2;
}

.updates-panel {
  display: grid;
  grid-template-columns: minmax(0, 0.95fr) minmax(18rem, 0.9fr);
  gap: 1.3rem;
  padding: 1.25rem;
}

.update-card {
  display: flex;
  gap: 1rem;
  align-items: center;
  min-width: 0;
}

.meta-line {
  color: #6b7588;
  font-size: 0.9rem;
}

.text-action {
  min-height: auto;
  padding: 0;
  border: 0;
  background: transparent;
  color: #3f48d8;
  cursor: pointer;
}

.timeline {
  display: grid;
  max-height: 15rem;
  overflow-y: auto;
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
  width: 1px;
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
}

.timeline-item.active .timeline-dot {
  background: #4f46e5;
  box-shadow: 0 0 0 0.34rem rgba(79, 70, 229, 0.13);
}

.timeline-item h3 {
  font-size: 0.98rem;
}

.timeline-item p {
  margin-top: 0.28rem;
  font-size: 0.92rem;
}

.work-grid,
.direction-grid {
  display: grid;
  gap: 1rem;
}

.work-grid {
  grid-template-columns: repeat(auto-fit, minmax(min(100%, 15rem), 1fr));
}

.direction-grid {
  grid-template-columns: repeat(auto-fit, minmax(min(100%, 15rem), 1fr));
}

.work-card {
  display: grid;
  gap: 1rem;
  align-content: start;
  min-height: 18rem;
  padding: 1rem;
}

.placeholder {
  border-style: dashed;
  background: rgba(250, 252, 255, 0.82);
}

.placeholder-icon,
.direction-icon {
  display: grid;
  place-items: center;
  width: 3rem;
  height: 3rem;
  border: 1px solid #dfe6f2;
  border-radius: 8px;
  background: #f7f9ff;
  color: #39445c;
  font-weight: 900;
}

.direction-card {
  min-height: 11rem;
  padding: 1rem;
}

.portfolio-footer {
  display: flex;
  justify-content: space-between;
  gap: 1rem;
  align-items: center;
  margin: 1.8rem 0 0.5rem;
  padding: 1.1rem 0 0;
  border-top: 1px solid #e2e7f1;
  color: #667187;
}

.portfolio-footer nav {
  display: flex;
  flex-wrap: wrap;
  gap: 1rem;
  align-items: center;
}

@media (max-width: 980px) {
  .portfolio-hero,
  .updates-panel {
    grid-template-columns: 1fr;
  }

  .portfolio-hero {
    gap: 2rem;
    min-height: auto;
    padding: 2.4rem 1.5rem 1.6rem;
  }
}

@media (max-width: 680px) {
  .portfolio-hero {
    padding: 2rem 0 1rem;
  }

  .hero-copy h1 {
    font-size: 2.1rem;
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
    min-height: auto;
    padding: 0.85rem;
  }

  .cover-frame.large {
    min-height: 12rem;
  }

  .updates-panel {
    padding: 0.85rem;
  }

  .update-card {
    flex-direction: column;
    align-items: flex-start;
  }

  .update-card .cover-frame {
    width: 100%;
    height: auto;
    aspect-ratio: 16 / 9;
    flex-basis: auto;
  }

  .work-card {
    min-height: auto;
  }

  .portfolio-footer {
    flex-direction: column;
    align-items: flex-start;
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
}
</style>
