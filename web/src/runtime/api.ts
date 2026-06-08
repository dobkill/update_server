type ApiPayload<T> = {
  code: number;
  message: string;
  data: T;
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
  github?: string;
};

export type ProjectScreenshot = {
  title: string;
  desc?: string;
  image: string;
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
  productCode?: string;
};

export type ProjectListData = {
  profile: SiteProfile;
  featured: PortfolioProject[];
  items: PortfolioProject[];
  filters: string[];
};

export type ProjectDetailData = {
  profile: SiteProfile;
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
  if (!response.ok || payload.code !== 0) {
    throw new Error(payload.message || `Request failed: ${response.status}`);
  }
  return payload.data;
}

export async function fetchProjects(): Promise<ProjectListData> {
  const data = await fetchJson<ProjectListData>("/api/v1/projects");

  if (!data.profile || !Array.isArray(data.items) || !data.items.every(isProject)) {
    throw new Error("项目列表接口返回结构不完整");
  }

  return {
    ...data,
    featured: Array.isArray(data.featured) ? data.featured.filter(isProject) : [],
    filters: Array.isArray(data.filters) ? data.filters : ["All"]
  };
}

export async function fetchProjectDetail(slug: string): Promise<ProjectDetailData> {
  const data = await fetchJson<ProjectDetailData>(`/api/v1/projects/${encodeURIComponent(slug)}`);
  if (!data.profile || !isProject(data.project)) {
    throw new Error("项目详情接口返回结构不完整");
  }
  return data;
}
