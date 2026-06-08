export const catalogPath = "/";
export const projectPathPrefix = "/projects";

export type AppLocation =
  | {
      page: "catalog";
    }
  | {
      page: "project";
      slug: string;
    };

export function normalizePath(pathname: string): string {
  if (!pathname || pathname === "/") {
    return catalogPath;
  }

  const withLeadingSlash = pathname.startsWith("/") ? pathname : `/${pathname}`;
  return withLeadingSlash.endsWith("/") ? withLeadingSlash.slice(0, -1) : withLeadingSlash;
}

export function normalizeSlug(slug?: string | null): string {
  return (slug ?? "").trim().toLowerCase();
}

export function resolveAppLocation(pathname: string): AppLocation {
  const normalizedPath = normalizePath(pathname);
  if (normalizedPath === catalogPath) {
    return { page: "catalog" };
  }

  const segments = normalizedPath
    .split("/")
    .filter(Boolean)
    .map((segment) => decodeURIComponent(segment));

  if (segments[0] === "projects") {
    const slug = normalizeSlug(segments[1]);
    return slug ? { page: "project", slug } : { page: "catalog" };
  }

  return { page: "catalog" };
}

export function getCurrentAppLocation(): AppLocation {
  return resolveAppLocation(window.location.pathname);
}

export function buildCatalogUrl(): string {
  return catalogPath;
}

export function buildProjectUrl(slug: string): string {
  const normalizedSlug = normalizeSlug(slug);
  return normalizedSlug ? `${projectPathPrefix}/${encodeURIComponent(normalizedSlug)}` : catalogPath;
}
