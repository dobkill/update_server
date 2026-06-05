export const catalogPath = "/";
export const latestVersionAlias = "latest";
export const defaultReleaseChannel = "stable";

export type AppLocation =
  | {
      page: "catalog";
    }
  | {
      page: "release";
      productCode: string;
      version: string;
      channel: string;
    };

export function normalizePath(pathname: string): string {
  if (!pathname || pathname === "/") {
    return catalogPath;
  }

  const withLeadingSlash = pathname.startsWith("/") ? pathname : `/${pathname}`;
  return withLeadingSlash.endsWith("/") ? withLeadingSlash.slice(0, -1) : withLeadingSlash;
}

export function normalizeProductCode(productCode?: string | null): string {
  return (productCode ?? "").trim();
}

export function normalizeVersion(version?: string | null): string {
  const normalized = (version ?? "").trim();
  return normalized || latestVersionAlias;
}

export function normalizeChannel(channel?: string | null): string {
  const normalized = (channel ?? "").trim().toLowerCase();
  return normalized || defaultReleaseChannel;
}

export function resolveAppLocation(pathname: string, search = ""): AppLocation {
  const normalizedPath = normalizePath(pathname);
  const channel = normalizeChannel(new URLSearchParams(search).get("channel"));

  if (normalizedPath === catalogPath) {
    return { page: "catalog" };
  }

  const segments = normalizedPath
    .split("/")
    .filter(Boolean)
    .map((segment) => decodeURIComponent(segment));

  const productCode = normalizeProductCode(segments[0]);
  const version = normalizeVersion(segments[1]);

  if (!productCode) {
    return { page: "catalog" };
  }

  return {
    page: "release",
    productCode,
    version,
    channel
  };
}

export function getCurrentAppLocation(): AppLocation {
  return resolveAppLocation(window.location.pathname, window.location.search);
}

export function buildCatalogUrl(): string {
  return catalogPath;
}

export function buildReleaseUrl(
  productCode: string,
  version = latestVersionAlias,
  channel = defaultReleaseChannel
): string {
  const normalizedProductCode = encodeURIComponent(normalizeProductCode(productCode));
  const normalizedVersion = normalizeVersion(version);

  if (!normalizedProductCode) {
    return catalogPath;
  }

  const basePath =
    normalizedVersion === latestVersionAlias
      ? `/${normalizedProductCode}`
      : `/${normalizedProductCode}/${encodeURIComponent(normalizedVersion)}`;

  if (normalizeChannel(channel) === defaultReleaseChannel) {
    return basePath;
  }

  const url = new URL(basePath, window.location.origin);
  url.searchParams.set("channel", normalizeChannel(channel));
  return `${url.pathname}${url.search}`;
}

export function attachProductCodeToHref(href: string): string {
  return href;
}
