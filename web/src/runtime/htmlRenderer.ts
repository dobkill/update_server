import type {
  FutureDirection,
  PageBlock,
  PortfolioHomeData,
  PortfolioProduct,
  ProductReleaseDetail,
  RecentUpdate,
  SiteProfile
} from "./api";
import { buildReleaseUrl } from "./location";

type BlockProps = Record<string, unknown>;

export type ShellRenderOptions = {
  currentProductCode?: string;
  currentVersion?: string;
  mobileMenuOpen: boolean;
  profile: SiteProfile;
};

const placeholderWorks = [
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

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === "object" && value !== null && !Array.isArray(value);
}

function text(value: unknown, fallback = ""): string {
  if (typeof value === "string") {
    return value;
  }
  if (typeof value === "number" || typeof value === "boolean") {
    return String(value);
  }
  return fallback;
}

function prop(props: BlockProps | undefined, key: string, fallback = ""): string {
  return text(props?.[key], fallback);
}

function records(value: unknown): Record<string, unknown>[] {
  return Array.isArray(value) ? value.filter(isRecord) : [];
}

export function escapeHtml(value: unknown): string {
  return text(value)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#39;");
}

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
  return [normalizeVersion(product.latest_version), formatDate(product.published_at || product.updated_at)]
    .filter(Boolean)
    .join(" · ");
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

function coverFrame(product: PortfolioProduct, sizeClass: "large" | "small" | "update"): string {
  const extraClass = sizeClass === "update" ? "" : ` ${sizeClass}`;
  if (product.cover_image_url) {
    return `
      <div class="cover-frame${extraClass}">
        <img src="${escapeHtml(product.cover_image_url)}" alt="${escapeHtml(product.name)}" />
      </div>
    `;
  }

  const markClass = sizeClass === "large" ? "daily-mark" : "daily-mark compact";
  return `
    <div class="cover-frame${extraClass}">
      <div class="${markClass}" aria-hidden="true">
        <span></span>
        <strong>Daily</strong>
      </div>
    </div>
  `;
}

function renderCardActions(product: PortfolioProduct): string {
  const detailAction = product.product_code
    ? `<a class="text-action" href="${escapeHtml(buildReleaseUrl(product.product_code))}" data-action="open-product" data-product-code="${escapeHtml(product.product_code)}">查看详情 <span aria-hidden="true">→</span></a>`
    : "";
  const githubAction = product.github_url
    ? `<a class="text-action" href="${escapeHtml(product.github_url)}" target="_blank" rel="noreferrer">GitHub <span aria-hidden="true">↗</span></a>`
    : "";

  if (!detailAction && !githubAction) {
    return "";
  }

  return `<div class="card-actions">${detailAction}${githubAction}</div>`;
}

function renderChipRow(product: PortfolioProduct): string {
  return `
    <div class="chip-row">
      <span>${escapeHtml(product.status_label || "已发布")}</span>
      <span>${escapeHtml(product.tag || "Obsidian 插件")}</span>
    </div>
  `;
}

function renderWorkCard(product: PortfolioProduct): string {
  return `
    <article class="work-card published">
      ${coverFrame(product, "small")}
      <div class="work-copy">
        ${renderChipRow(product)}
        <h3>${escapeHtml(product.name)}</h3>
        <p>${escapeHtml(product.summary)}</p>
        <span class="meta-line">${escapeHtml(productMeta(product))}</span>
        ${renderCardActions(product)}
      </div>
    </article>
  `;
}

function renderPlaceholderWork(item: (typeof placeholderWorks)[number]): string {
  return `
    <article class="work-card placeholder">
      <div class="placeholder-icon" aria-hidden="true">${escapeHtml(item.icon)}</div>
      <div class="work-copy">
        <div class="chip-row muted-chip"><span>${escapeHtml(item.status)}</span></div>
        <h3>${escapeHtml(item.title)}</h3>
        <p>${escapeHtml(item.summary)}</p>
      </div>
    </article>
  `;
}

export function renderShell(content: string, options: ShellRenderOptions): string {
  const githubUrl = options.profile.github_url || "";
  const mobileClass = options.mobileMenuOpen ? " open" : "";
  const mobileNav = options.mobileMenuOpen
    ? `
      <nav class="mobile-nav" aria-label="移动导航">
        <a class="active" href="/#works" data-action="close-menu">作品</a>
        ${
          githubUrl
            ? `<a href="${escapeHtml(githubUrl)}" target="_blank" rel="noreferrer" data-action="close-menu">GitHub ↗</a>`
            : ""
        }
      </nav>
    `
    : "";

  const releaseContext =
    options.currentProductCode || options.currentVersion
      ? `
        <div class="release-context">
          ${options.currentProductCode ? `<span>${escapeHtml(options.currentProductCode)}</span>` : ""}
          ${options.currentVersion ? `<span>v${escapeHtml(options.currentVersion.replace(/^v/, ""))}</span>` : ""}
        </div>
      `
      : "";

  return `
    <div class="shell">
      <header class="topbar-wrapper">
        <div class="topbar">
          <button type="button" class="brand" data-action="home">
            <span class="brand-mark" aria-hidden="true"><span></span></span>
            <span class="brand-copy">
              <strong>${escapeHtml(options.profile.site_name || "YXX Works")}</strong>
              <span>${escapeHtml(options.profile.subtitle || "产品、插件与创作实验")}</span>
            </span>
          </button>

          <nav class="topbar-nav desktop-nav" aria-label="主导航">
            <a class="active" href="/#works">作品</a>
            ${githubUrl ? `<a href="${escapeHtml(githubUrl)}" target="_blank" rel="noreferrer">GitHub ↗</a>` : ""}
          </nav>

          <button type="button" class="hamburger${mobileClass}" aria-label="菜单" aria-expanded="${options.mobileMenuOpen}" data-action="toggle-menu">
            <span></span>
            <span></span>
            <span></span>
          </button>
        </div>
        ${mobileNav}
      </header>

      ${releaseContext}

      <main class="content">
        ${content}
      </main>
    </div>
  `;
}

export function renderLoadingState(): string {
  return `
    <section class="state-card loading-state">
      <div class="spinner" aria-hidden="true"><span></span></div>
      <div class="state-copy">
        <strong>加载中</strong>
        <span>正在读取发布数据...</span>
      </div>
    </section>
  `;
}

export function renderErrorState(message: string): string {
  return `
    <section class="state-card error-state">
      <div class="error-icon" aria-hidden="true">!</div>
      <div class="state-copy">
        <strong>提示</strong>
        <span>${escapeHtml(message)}</span>
      </div>
    </section>
  `;
}

export function renderSoftErrorBar(message: string): string {
  return `<div class="soft-error-bar"><span>${escapeHtml(message)}</span></div>`;
}

export function renderCatalogPage(home: PortfolioHomeData): string {
  const primaryWork = home.recommendations[0] ?? home.products[0] ?? null;
  const realUpdates = home.recent_updates.filter((item) => item.title && item.description);
  const recentUpdates: RecentUpdate[] = [
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
  const latestUpdate = home.recent_updates[0] ?? null;
  const directions = home.future_directions.length ? home.future_directions : fallbackDirections;

  const heroFeature = primaryWork
    ? `
      <div class="hero-feature" aria-label="主推项目">
        <div class="ghost-card one"></div>
        <div class="ghost-card two"></div>
        <article class="spotlight-card">
          ${coverFrame(primaryWork, "large")}
          <div class="spotlight-copy">
            <span class="eyebrow">主推项目</span>
            <h2>${escapeHtml(primaryWork.name)}</h2>
            ${renderChipRow(primaryWork)}
            <p>${escapeHtml(primaryWork.summary)}</p>
          </div>
        </article>
      </div>
    `
    : "";

  const updatesSection = primaryWork
    ? `
      <section class="portfolio-section">
        <header class="section-heading"><h2>最近更新</h2></header>
        <div class="updates-panel">
          <article class="update-card">
            ${coverFrame(primaryWork, "update")}
            <div class="update-copy">
              ${renderChipRow(primaryWork)}
              <h3>${escapeHtml(primaryWork.name)}</h3>
              <p>${escapeHtml(primaryWork.summary)}</p>
              <span class="meta-line">${escapeHtml(updateMeta(latestUpdate, primaryWork))}</span>
              ${renderCardActions(primaryWork)}
            </div>
          </article>

          <div class="portfolio-timeline" aria-label="更新时间线">
            ${recentUpdates
              .map(
                (item, index) => `
                  <article class="timeline-item${index === 0 ? " active" : ""}">
                    <span class="timeline-dot" aria-hidden="true"></span>
                    <div>
                      <h3>${escapeHtml(item.title)}</h3>
                      <p>${escapeHtml(item.description)}</p>
                    </div>
                  </article>
                `
              )
              .join("")}
          </div>
        </div>
      </section>
    `
    : "";

  return `
    <section class="portfolio-hero">
      <div class="portfolio-hero-copy">
        <h1>个人作品集</h1>
        <p>设计、开发与持续迭代中的作品。</p>
        <div class="hero-actions" aria-label="首页操作">
          <a class="primary-action" href="#works">浏览作品 <span aria-hidden="true">→</span></a>
          ${
            home.profile.github_url
              ? `<a class="secondary-action" href="${escapeHtml(home.profile.github_url)}" target="_blank" rel="noreferrer">查看 GitHub <span aria-hidden="true">↗</span></a>`
              : ""
          }
        </div>
      </div>
      ${heroFeature}
    </section>

    ${updatesSection}

    <section class="portfolio-section" id="works">
      <header class="section-heading"><h2>全部作品</h2></header>
      <div class="work-grid">
        ${home.products.map(renderWorkCard).join("")}
        ${placeholderWorks.map(renderPlaceholderWork).join("")}
      </div>
    </section>

    <section class="portfolio-section">
      <header class="section-heading"><h2>未来方向</h2></header>
      <div class="direction-grid">
        ${directions
          .map(
            (item, index) => `
              <article class="direction-card">
                <span class="direction-icon" aria-hidden="true">${escapeHtml(directionIcon(index))}</span>
                <h3>${escapeHtml(item.title)}</h3>
                <p>${escapeHtml(item.comment)}</p>
              </article>
            `
          )
          .join("")}
      </div>
    </section>

    <footer class="portfolio-footer">
      <div class="footer-brand">
        <span class="footer-logo" aria-hidden="true"><span></span></span>
        <span>© ${new Date().getFullYear()} ${escapeHtml(home.profile.site_name || "YXX Works")}</span>
      </div>
      ${
        home.profile.github_url || home.profile.email
          ? `
            <nav aria-label="站点链接" class="footer-nav">
              ${home.profile.github_url ? `<a href="${escapeHtml(home.profile.github_url)}" target="_blank" rel="noreferrer">GitHub</a>` : ""}
              ${home.profile.email ? `<a href="mailto:${escapeHtml(home.profile.email)}">邮件联系</a>` : ""}
            </nav>
          `
          : ""
      }
    </footer>
  `;
}

export function renderReleasePage(release: ProductReleaseDetail, blocks: PageBlock[]): string {
  const publishedDate = release.published_at?.slice(0, 10) ?? "--";

  return `
    <section class="release-toolbar">
      <button type="button" class="back-link" data-action="back">
        <span aria-hidden="true">←</span> 返回
      </button>

      <div class="release-title">
        <p>${escapeHtml(release.product_code)}</p>
        <h1>${escapeHtml(release.title)}</h1>
      </div>

      <dl class="release-meta">
        <div>
          <dt>版本</dt>
          <dd>${escapeHtml(release.resolved_version)}</dd>
        </div>
        <div>
          <dt>渠道</dt>
          <dd>${escapeHtml(release.channel)}</dd>
        </div>
        <div>
          <dt>发布</dt>
          <dd>${escapeHtml(publishedDate)}</dd>
        </div>
      </dl>
    </section>

    ${renderBlocks(blocks)}
  `;
}

function renderBlocks(blocks: PageBlock[]): string {
  let featureGridIdUsed = false;
  const blockHtml = blocks
    .map((block, index) => {
      const useFeatureId = block.type === "FeatureGridBlock" && !featureGridIdUsed;
      featureGridIdUsed = featureGridIdUsed || useFeatureId;
      return renderBlock(block, index, useFeatureId);
    })
    .join("");

  return `<div class="blocks">${blockHtml}</div>`;
}

function renderBlock(block: PageBlock, index: number, useFeatureId: boolean): string {
  const props = block.props ?? {};

  switch (block.type) {
    case "HeroBlock":
      return renderHeroBlock(props);
    case "FeatureGridBlock":
      return renderFeatureGridBlock(props, useFeatureId);
    case "RichTextBlock":
      return renderRichTextBlock(props);
    case "ImageTextBlock":
      return renderImageTextBlock(props);
    case "ScreenshotGalleryBlock":
      return renderScreenshotGalleryBlock(props);
    case "DownloadPanelBlock":
      return renderDownloadPanelBlock(props);
    case "TimelineBlock":
      return renderTimelineBlock(props);
    case "FaqBlock":
      return renderFaqBlock(props);
    case "FooterCtaBlock":
      return renderFooterCtaBlock(props);
    default:
      return renderRichTextBlock({
        title: block.type || `区块 ${index + 1}`,
        content: "当前前端不认识这个区块类型，已按文本区块兜底展示。"
      });
  }
}

function renderHeroBlock(props: BlockProps): string {
  const image = prop(props, "image");
  const secondaryText = prop(props, "secondaryText");
  const secondaryHref = prop(props, "secondaryHref");

  return `
    <section class="block-hero">
      <div class="block-hero-copy">
        <p class="kicker">${escapeHtml(prop(props, "eyebrow", "Release"))}</p>
        <h2>${escapeHtml(prop(props, "title", "未配置标题"))}</h2>
        <p>${escapeHtml(prop(props, "subtitle"))}</p>
        <div class="block-actions">
          <a href="${escapeHtml(prop(props, "primaryHref", "/"))}">${escapeHtml(prop(props, "primaryText", "了解更多"))}</a>
          ${
            secondaryText && secondaryHref
              ? `<a class="secondary" href="${escapeHtml(secondaryHref)}">${escapeHtml(secondaryText)}</a>`
              : ""
          }
        </div>
      </div>
      ${image ? `<img class="block-hero-image" src="${escapeHtml(image)}" alt="${escapeHtml(prop(props, "title", "发布页图片"))}" />` : ""}
    </section>
  `;
}

function renderFeatureGridBlock(props: BlockProps, useFeatureId: boolean): string {
  const items = records(props.items);
  return `
    <section class="block-feature-grid"${useFeatureId ? ' id="features"' : ""}>
      ${items
        .map(
          (item) => `
            <article class="block-feature-card">
              <h3>${escapeHtml(text(item.title))}</h3>
              <p>${escapeHtml(text(item.desc))}</p>
            </article>
          `
        )
        .join("")}
    </section>
  `;
}

function renderRichTextBlock(props: BlockProps): string {
  return `
    <section class="block-richtext">
      <h3>${escapeHtml(prop(props, "title", "说明"))}</h3>
      <p>${escapeHtml(prop(props, "content"))}</p>
    </section>
  `;
}

function renderImageTextBlock(props: BlockProps): string {
  const title = prop(props, "title", "图文区块");
  const image = prop(props, "image");
  return `
    <section class="block-image-text">
      <div class="block-image-copy">
        <h3>${escapeHtml(title)}</h3>
        <p>${escapeHtml(prop(props, "content"))}</p>
      </div>
      <div class="block-image-art">
        ${image ? `<img src="${escapeHtml(image)}" alt="${escapeHtml(title)}" />` : `<div class="block-placeholder">可接入发布后的静态资源</div>`}
      </div>
    </section>
  `;
}

function renderScreenshotGalleryBlock(props: BlockProps): string {
  const title = prop(props, "title");
  const intro = prop(props, "intro");
  const items = records(props.items);

  return `
    <section class="block-gallery">
      <div class="block-gallery-head">
        <h3>${escapeHtml(title)}</h3>
        ${intro ? `<p>${escapeHtml(intro)}</p>` : ""}
      </div>

      <div class="block-gallery-grid">
        ${items
          .map(
            (item) => `
              <article class="block-shot-card">
                <div class="block-shot-image-wrap">
                  <img src="${escapeHtml(text(item.image))}" alt="${escapeHtml(text(item.title))}" loading="lazy" />
                  <div class="block-shot-overlay"><span>${escapeHtml(text(item.title))}</span></div>
                </div>
                <div class="block-shot-info">
                  <h4>${escapeHtml(text(item.title))}</h4>
                  ${text(item.desc) ? `<p>${escapeHtml(text(item.desc))}</p>` : ""}
                </div>
              </article>
            `
          )
          .join("")}
      </div>
    </section>
  `;
}

function renderDownloadPanelBlock(props: BlockProps): string {
  const downloadUrl = prop(props, "downloadUrl");
  return `
    <section class="block-download-panel">
      <div class="block-panel-info">
        <p class="tag">当前推荐版本</p>
        <h3>${escapeHtml(prop(props, "version", "unknown"))}</h3>
        <p>${escapeHtml(prop(props, "releaseNote"))}</p>
      </div>
      <dl>
        <div>
          <dt>包类型</dt>
          <dd>${escapeHtml(prop(props, "packageType", "full"))}</dd>
        </div>
        <div>
          <dt>体积</dt>
          <dd>${escapeHtml(prop(props, "packageSize", "0 MB"))}</dd>
        </div>
      </dl>
      ${
        downloadUrl
          ? `<a class="download-link" href="${escapeHtml(downloadUrl)}">${escapeHtml(prop(props, "downloadText", "立即下载"))}</a>`
          : ""
      }
    </section>
  `;
}

function renderTimelineBlock(props: BlockProps): string {
  const items = Array.isArray(props.items) ? props.items : [];
  return `
    <section class="block-timeline">
      ${items
        .map(
          (item, index) => `
            <div class="block-timeline-row">
              <span class="step-num">${index + 1}</span>
              <p>${escapeHtml(item)}</p>
            </div>
          `
        )
        .join("")}
    </section>
  `;
}

function renderFaqBlock(props: BlockProps): string {
  const items = records(props.items);
  return `
    <section class="block-faq">
      ${items
        .map(
          (item) => `
            <article class="block-faq-item">
              <h3>${escapeHtml(text(item.question))}</h3>
              <p>${escapeHtml(text(item.answer))}</p>
            </article>
          `
        )
        .join("")}
    </section>
  `;
}

function renderFooterCtaBlock(props: BlockProps): string {
  return `
    <section class="block-footer-cta">
      <div class="block-cta-copy">
        <h3>${escapeHtml(prop(props, "title", "继续扩展页面能力"))}</h3>
        <p>${escapeHtml(prop(props, "desc"))}</p>
      </div>
      <a class="block-cta-button" href="${escapeHtml(prop(props, "actionHref", "/"))}">${escapeHtml(prop(props, "actionText", "前往"))}</a>
    </section>
  `;
}
