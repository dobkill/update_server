type ApiPayload<T> = {
  code: number;
  message: string;
  request_id?: string;
  data: T;
};

export type ReleaseChannel = string;
export const defaultReleaseChannel = "stable";

export type PageBlock = {
  type: string;
  props?: Record<string, unknown>;
};

export type ProductSummary = {
  product_code: string;
  name: string;
  summary: string;
  cover_image_url?: string;
  github_url?: string;
  latest_version?: string;
  updated_at?: string;
  tag?: string;
  status_label?: string;
};

export type SiteProfile = {
  site_name: string;
  subtitle: string;
  github_url: string;
  email: string;
};

export type PortfolioProduct = ProductSummary & {
  published_at?: string;
};

export type RecentUpdate = {
  product_code: string;
  name: string;
  version: string;
  published_at: string;
  title: string;
  description: string;
};

export type FutureDirection = {
  title: string;
  comment: string;
  icon_path?: string;
};

export type PortfolioHomeData = {
  profile: SiteProfile;
  recommendations: PortfolioProduct[];
  products: PortfolioProduct[];
  recent_updates: RecentUpdate[];
  future_directions: FutureDirection[];
};

export type ReleasePageData = {
  blocks?: PageBlock[];
  hero?: {
    title?: string;
    subtitle?: string;
    primary_text?: string;
    primary_href?: string;
  };
  summary?: {
    title?: string;
    content?: string;
  };
  highlights?: {
    items?: Array<{
      title: string;
      desc: string;
    }>;
  };
  timeline?: {
    items?: string[];
  };
  download?: {
    version?: string;
    package_type?: string;
    package_size?: string;
    release_note?: string;
    download_text?: string;
    download_url?: string;
    platform?: string;
    arch?: string;
    channel?: string;
  };
  faq?: {
    items?: Array<{
      question: string;
      answer: string;
    }>;
  };
  footer_cta?: {
    title?: string;
    desc?: string;
    action_text?: string;
    action_href?: string;
  };
};

export type ProductReleaseDetail = {
  product_code: string;
  requested_version: string;
  resolved_version: string;
  channel: string;
  title: string;
  published_at: string;
  release_notes_summary: string;
  page: {
    vue_entry_url?: string;
    page_data?: ReleasePageData;
  };
};

export async function fetchProducts(): Promise<ProductSummary[] | null> {
  try {
    const response = await fetch("/api/v1/products", {
      headers: {
        Accept: "application/json"
      }
    });

    if (!response.ok) {
      return null;
    }

    const payload = (await response.json()) as ApiPayload<{
      items?: ProductSummary[];
    }>;

    if (payload.code !== 0 || !Array.isArray(payload.data?.items)) {
      return null;
    }

    return payload.data.items;
  } catch (error) {
    console.warn("fetchProducts fallback", error);
    return null;
  }
}

export async function fetchPortfolioHome(): Promise<PortfolioHomeData | null> {
  try {
    const response = await fetch("/api/v1/portfolio-home", {
      headers: {
        Accept: "application/json"
      }
    });

    if (!response.ok) {
      return null;
    }

    const payload = (await response.json()) as ApiPayload<PortfolioHomeData>;

    if (
      payload.code !== 0 ||
      !payload.data?.profile ||
      !Array.isArray(payload.data.products) ||
      !Array.isArray(payload.data.recommendations) ||
      !Array.isArray(payload.data.recent_updates) ||
      !Array.isArray(payload.data.future_directions)
    ) {
      return null;
    }

    return payload.data;
  } catch (error) {
    console.warn("fetchPortfolioHome fallback", error);
    return null;
  }
}

export async function fetchReleaseDetail(
  productCode: string,
  version: string,
  channel = defaultReleaseChannel
): Promise<ProductReleaseDetail | null> {
  try {
    const path = `/api/v1/products/${encodeURIComponent(productCode)}/Document`;
    const url = new URL(path, window.location.origin);
    url.searchParams.set("version", version || "latest");
    url.searchParams.set("channel", channel);

    const response = await fetch(url.pathname + url.search, {
      headers: {
        Accept: "application/json"
      }
    });

    if (!response.ok) {
      return null;
    }

    const payload = (await response.json()) as ApiPayload<ProductReleaseDetail>;

    if (
      payload.code !== 0 ||
      !payload.data?.product_code ||
      !payload.data?.requested_version ||
      !payload.data?.resolved_version
    ) {
      return null;
    }

    return payload.data;
  } catch (error) {
    console.warn("fetchReleaseDetail fallback", error);
    return null;
  }
}
