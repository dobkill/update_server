import type { PortfolioProject, ProjectListData, ProjectScreenshot, SiteProfile } from "./api";
import { buildCatalogUrl, buildProjectUrl } from "./location";

function escapeHtml(value: unknown): string {
  return String(value ?? "")
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#039;");
}

function linkOrEmpty(href: string | undefined, label: string): string {
  if (!href) {
    return "";
  }

  const isEmail = href.includes("@") && !href.startsWith("http");
  const target = isEmail ? `mailto:${href}` : href;
  const external = target.startsWith("http");

  return `
    <a class="nav-link" href="${escapeHtml(target)}"${external ? ' target="_blank" rel="noreferrer"' : ""}>
      ${escapeHtml(label)}
    </a>
  `;
}

function techTags(project: PortfolioProject, limit = 5): string {
  return project.techStack
    .slice(0, limit)
    .map((item) => `<span class="tag">${escapeHtml(item)}</span>`)
    .join("");
}

function projectImageUrl(project: PortfolioProject, variant = "card"): string {
  if (variant === "detail") {
    return project.heroImageUrl || project.coverImageUrl || project.screenshots[0]?.image || "";
  }
  return project.coverImageUrl || project.heroImageUrl || project.screenshots[0]?.image || "";
}

function projectMedia(project: PortfolioProject, variant = "card", loading: "eager" | "lazy" = "lazy"): string {
  const image = projectImageUrl(project, variant);
  if (!image) {
    return `
      <div class="project-media ${variant} empty">
        <span>${escapeHtml(project.category)}</span>
        <strong>${escapeHtml(project.name)}</strong>
      </div>
    `;
  }

  return `
    <figure class="project-media ${variant}">
      <img src="${escapeHtml(image)}" alt="${escapeHtml(project.name)} screenshot" loading="${loading}" />
    </figure>
  `;
}

function renderHeroPreview(project: PortfolioProject | undefined): string {
  if (!project) {
    return "";
  }

  return `
    <a class="hero-preview" href="${buildProjectUrl(project.slug)}" data-action="open-project" data-slug="${escapeHtml(project.slug)}">
      ${projectMedia(project, "hero", "eager")}
      <div>
        <span>${escapeHtml(project.category)}</span>
        <strong>${escapeHtml(project.name)}</strong>
        <p>${escapeHtml(project.description)}</p>
      </div>
    </a>
  `;
}

function renderScreenshot(screenshot: ProjectScreenshot, index: number): string {
  return `
    <article class="${index === 0 ? "wide" : ""}">
      <figure class="screenshot-media">
        <img src="${escapeHtml(screenshot.image)}" alt="${escapeHtml(screenshot.title)}" loading="lazy" />
      </figure>
      <h3>${escapeHtml(screenshot.title)}</h3>
      ${screenshot.desc ? `<p>${escapeHtml(screenshot.desc)}</p>` : ""}
    </article>
  `;
}

export function renderShell(content: string, profile: SiteProfile): string {
  return `
    <div class="site-shell">
      <header class="site-header">
        <a class="brand" href="${buildCatalogUrl()}" data-action="home">${escapeHtml(profile.ownerName)}</a>
        <nav class="site-nav" aria-label="External links">
          ${linkOrEmpty(profile.githubUrl, "GitHub")}
          ${linkOrEmpty(profile.email, "Email")}
          ${linkOrEmpty(profile.resumeUrl, "Resume")}
        </nav>
      </header>
      <main>${content}</main>
    </div>
  `;
}

export function renderLoadingState(): string {
  return `
    <section class="state-panel">
      <span class="spinner"></span>
      <strong>Loading project lab</strong>
    </section>
  `;
}

export function renderErrorState(message: string): string {
  return `
    <section class="state-panel error">
      <strong>Unable to load</strong>
      <p>${escapeHtml(message)}</p>
    </section>
  `;
}

function renderFeaturedProjects(projects: PortfolioProject[]): string {
  if (!projects.length) {
    return "";
  }

  const [primary, ...secondary] = projects;
  return `
    <section class="section-band" id="featured">
      <div class="section-heading">
        <p>FEATURED PROJECT</p>
        <h2>Built like products, documented like case studies.</h2>
      </div>
      <div class="featured-layout">
        <a class="featured-card" href="${buildProjectUrl(primary.slug)}" data-action="open-project" data-slug="${escapeHtml(primary.slug)}">
          ${projectMedia(primary, "large")}
          <div>
            <h3>${escapeHtml(primary.name)}</h3>
            <p>${escapeHtml(primary.description)}</p>
            <div class="tag-row">${techTags(primary)}</div>
          </div>
        </a>
        <div class="featured-stack">
          ${secondary
            .slice(0, 3)
            .map(
              (project) => `
                <a class="mini-project" href="${buildProjectUrl(project.slug)}" data-action="open-project" data-slug="${escapeHtml(project.slug)}">
                  ${projectMedia(project, "small")}
                  <div>
                    <span>${escapeHtml(project.category)}</span>
                    <h3>${escapeHtml(project.name)}</h3>
                    <p>${escapeHtml(project.description)}</p>
                    <div class="tag-row compact">${techTags(project, 3)}</div>
                  </div>
                </a>
              `
            )
            .join("")}
        </div>
      </div>
    </section>
  `;
}

function renderProjectGrid(projects: PortfolioProject[], filters: string[], selectedFilter: string, searchTerm: string): string {
  return `
    <section class="section-band" id="projects">
      <div class="catalog-tools">
        <div class="section-heading compact">
          <p>ALL PROJECTS</p>
          <h2>${projects.length} projects in the lab</h2>
        </div>
        <div class="project-controls">
          <div class="filter-tabs" role="tablist">
            ${filters
              .map(
                (filter) => `
                  <button type="button" class="${filter === selectedFilter ? "active" : ""}" data-action="set-filter" data-filter="${escapeHtml(filter)}">
                    ${escapeHtml(filter)}
                  </button>
                `
              )
              .join("")}
          </div>
          <input class="project-search" type="search" value="${escapeHtml(searchTerm)}" placeholder="Search projects..." data-action="search-projects" />
        </div>
      </div>
      <div class="project-grid">
        ${projects
          .map(
            (project) => `
              <a class="project-card" href="${buildProjectUrl(project.slug)}" data-action="open-project" data-slug="${escapeHtml(project.slug)}">
                ${projectMedia(project)}
                <div class="card-meta">
                  <span>${escapeHtml(project.category)}</span>
                  <strong>${escapeHtml(project.status)}</strong>
                </div>
                <h3>${escapeHtml(project.name)}</h3>
                <p>${escapeHtml(project.description)}</p>
                <div class="tag-row compact">${techTags(project, 4)}</div>
              </a>
            `
          )
          .join("")}
      </div>
    </section>
  `;
}

export function renderCatalogPage(
  data: ProjectListData,
  visibleProjects: PortfolioProject[],
  selectedFilter: string,
  searchTerm: string
): string {
  const profile = data.profile;
  const featured = data.featured.length ? data.featured : data.items.filter((item) => item.featured);

  return `
    <section class="hero">
      <div class="hero-copy">
        <span>${escapeHtml(profile.heroLabel)}</span>
        <h1>${escapeHtml(profile.heroTitle)}</h1>
        <p>${escapeHtml(profile.heroDescription)}</p>
        <div class="hero-actions">
          <a class="primary-button" href="#featured">View Featured Projects</a>
          <a class="secondary-button" href="#projects">Browse All Projects</a>
        </div>
      </div>
      ${renderHeroPreview(featured[0] ?? data.items[0])}
    </section>
    ${renderFeaturedProjects(featured)}
    ${renderProjectGrid(visibleProjects, data.filters, selectedFilter, searchTerm)}
    <footer class="site-footer">
      <span>© 2026 ${escapeHtml(profile.ownerName)}. All rights reserved.</span>
      <nav>
        ${linkOrEmpty(profile.githubUrl, "GitHub")}
        ${linkOrEmpty(profile.linkedinUrl, "LinkedIn")}
        ${linkOrEmpty(profile.twitterUrl, "Twitter")}
        ${linkOrEmpty(profile.email, "Email")}
      </nav>
      <span>Built with TypeScript & CSS</span>
    </footer>
  `;
}

function renderMeta(project: PortfolioProject): string {
  const meta = [
    ["Role", project.role],
    ["Type", project.type],
    ["Year", project.year],
    ["Status", project.status],
    ["Platform", project.platform]
  ];

  return `
    <dl class="project-meta">
      ${meta
        .map(
          ([label, value]) => `
            <div>
              <dt>${escapeHtml(label)}</dt>
              <dd>${escapeHtml(value)}</dd>
            </div>
          `
        )
        .join("")}
    </dl>
  `;
}

function renderActionLinks(project: PortfolioProject): string {
  const links = [
    ["Live Demo", project.links.liveDemo],
    ["View on GitHub", project.links.github]
  ];

  return links
    .filter(([, href]) => href)
    .map(
      ([label, href], index) => `
        <a class="${index === 0 ? "primary-button light" : "secondary-button light"}" href="${escapeHtml(href)}" target="_blank" rel="noreferrer">
          ${escapeHtml(label)}
        </a>
      `
    )
    .join("");
}

export function renderProjectDetail(project: PortfolioProject, profile: SiteProfile): string {
  return `
    <article class="project-page">
      <header class="project-header">
        <nav class="detail-nav">
          <a href="${buildCatalogUrl()}" data-action="home">${escapeHtml(profile.ownerName)}</a>
          <a href="${buildCatalogUrl()}#projects">Back to Projects</a>
        </nav>
        <div class="project-hero">
          <div>
            <span>${project.featured ? "FEATURED PROJECT" : escapeHtml(project.category)}</span>
            <h1>${escapeHtml(project.name)}</h1>
            <p>${escapeHtml(project.longDescription || project.description)}</p>
            <div class="tag-row detail-tags">${techTags(project, 10)}</div>
            <div class="hero-actions">${renderActionLinks(project)}</div>
          </div>
          ${projectMedia(project, "detail", "eager")}
        </div>
        ${renderMeta(project)}
      </header>

      <section class="case-section overview-grid">
        <div>
          <p class="eyebrow">OVERVIEW</p>
          <h2>${escapeHtml(project.description)}</h2>
          <p>${escapeHtml(project.longDescription)}</p>
        </div>
        <div class="feature-grid">
          ${project.features
            .map(
              (feature) => `
                <article>
                  <span></span>
                  <h3>${escapeHtml(feature)}</h3>
                </article>
              `
            )
            .join("")}
        </div>
      </section>

      <section class="case-section">
        <div class="section-heading light">
          <p>SCREENSHOTS</p>
          <h2>Interface moments</h2>
        </div>
        <div class="screenshot-grid">
          ${project.screenshots.map(renderScreenshot).join("")}
        </div>
      </section>

      <section class="case-section">
        <div class="section-heading light">
          <p>TECH STACK</p>
          <h2>Tools behind the build</h2>
        </div>
        <div class="stack-cloud">${project.techStack.map((item) => `<span>${escapeHtml(item)}</span>`).join("")}</div>
      </section>

      <section class="case-section">
        <div class="section-heading light">
          <p>ARCHITECTURE</p>
          <h2>System flow</h2>
        </div>
        <div class="architecture-flow">
          ${project.architecture
            .map(
              (node, index) => `
                <div>
                  <span>${String(index + 1).padStart(2, "0")}</span>
                  <strong>${escapeHtml(node)}</strong>
                </div>
              `
            )
            .join("")}
        </div>
      </section>

      <section class="case-section case-cards">
        ${[
          ["Challenge", project.challenge],
          ["Solution", project.solution],
          ["Result", project.result]
        ]
          .map(
            ([title, body]) => `
              <article>
                <h3>${escapeHtml(title)}</h3>
                <p>${escapeHtml(body)}</p>
              </article>
            `
          )
          .join("")}
      </section>

      ${
        Array.isArray(project.pages) && project.pages.length
          ? `
        <section class="case-section">
          <div class="section-heading light">
            <p>PROJECT PAGES</p>
            <h2>Exclusive showcase pages</h2>
          </div>
          <div class="architecture-flow">
            ${project.pages
              .map(
                (page) => `
                  <a href="${escapeHtml(page.htmlUrl)}" class="page-link-card">
                    <span>${escapeHtml(page.title)}</span>
                    <strong>${escapeHtml(page.summary || page.slug)}</strong>
                  </a>
                `
              )
              .join("")}
          </div>
        </section>
      `
          : ""
      }
    </article>
  `;
}
