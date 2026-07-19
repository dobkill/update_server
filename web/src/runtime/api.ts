type ApiPayload<T> = {
  success: boolean;
  data: T;
  error?: { code: string; message: string; details?: unknown };
};

export type SiteProfile = {
  ownerName: string;
  siteName: string;
  subtitle: string;
  heroLabel: string;
  heroTitle: string;
  heroDescription: string;
  githubUrl: string;
  email: string;
  resumeUrl: string;
  linkedinUrl: string;
  twitterUrl: string;
};

export type ProjectLinks = {
  liveDemo?: string;
  demo?: string;
  github?: string;
  download?: string;
};

export type ProjectScreenshot = {
  title: string;
  desc?: string;
  image: string;
};

export type ProjectPage = {
  slug: string;
  title: string;
  summary: string;
  sortOrder: number;
  projectSlug: string;
  htmlUrl: string;
};

export type PortfolioProject = {
  slug: string;
  name: string;
  category: string;
  description: string;
  longDescription: string;
  featured: boolean;
  status: string;
  year: string;
  platform: string;
  role: string;
  type: string;
  coverImageUrl?: string;
  heroImageUrl?: string;
  techStack: string[];
  features: string[];
  screenshots: ProjectScreenshot[];
  architecture: string[];
  challenge: string;
  solution: string;
  result: string;
  links: ProjectLinks;
  sortOrder: number;
  pages?: ProjectPage[];
};

export type ProjectListData = {
  profile: SiteProfile;
  featured: PortfolioProject[];
  items: PortfolioProject[];
  filters: string[];
};

export type ProjectDetailData = {
  project: PortfolioProject;
};

export const defaultProfile: SiteProfile = {
  ownerName: "Xiang Y.",
  siteName: "Personal Software Lab",
  subtitle: "Self-built apps, tools, systems, and experiments.",
  heroLabel: "DEVELOPER & BUILDER",
  heroTitle: "Personal Software Lab",
  heroDescription:
    "A collection of self-built apps, tools, systems, and experiments — crafted with code and curiosity.",
  githubUrl: "",
  email: "",
  resumeUrl: "",
  linkedinUrl: "",
  twitterUrl: ""
};

function isProject(value: unknown): value is PortfolioProject {
  const candidate = value as PortfolioProject;
  return Boolean(
    candidate &&
      typeof candidate.slug === "string" &&
      typeof candidate.name === "string" &&
      Array.isArray(candidate.techStack) &&
      Array.isArray(candidate.features) &&
      Array.isArray(candidate.screenshots) &&
      candidate.screenshots.every(
        (item) =>
          item &&
          typeof item.title === "string" &&
          typeof item.image === "string" &&
          (item.desc === undefined || typeof item.desc === "string")
      )
  );
}

async function fetchJson<T>(url: string): Promise<T> {
  const response = await fetch(url, {
    headers: {
      Accept: "application/json"
    }
  });

  const payload = (await response.json()) as ApiPayload<T>;
  if (!response.ok || !payload.success) {
    const message = payload.error?.message || `Request failed: ${response.status}`;
    throw new Error(message);
  }
  return payload.data;
}

export async function fetchHome(): Promise<ProjectListData> {
  const data = await fetchJson<ProjectListData>("/api/v1/home");

  const profile = data.profile ?? defaultProfile;
  const items = Array.isArray(data.items) ? data.items.filter(isProject) : [];
  const featured = Array.isArray(data.featured)
    ? data.featured.filter(isProject)
    : items.filter((item) => item.featured);
  const filters = Array.isArray(data.filters) ? data.filters : ["All"];

  return { profile, featured, items, filters };
}

export async function fetchProjects(): Promise<PortfolioProject[]> {
  const data = await fetchJson<{ items: PortfolioProject[] }>("/api/v1/projects");
  return Array.isArray(data.items) ? data.items.filter(isProject) : [];
}

export async function fetchProjectDetail(slug: string): Promise<ProjectDetailData> {
  const data = await fetchJson<ProjectDetailData>(
    `/api/v1/projects/${encodeURIComponent(slug)}`
  );
  if (!isProject(data.project)) {
    throw new Error("项目详情接口返回结构不完整");
  }
  return data;
}
