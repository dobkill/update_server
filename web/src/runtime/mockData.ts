import productsJson from "../mock/products.json";
import portfolioHomeJson from "../mock/portfolioHome.json";
import releasesJson from "../mock/releases.json";
import {
  defaultReleaseChannel,
  type PortfolioHomeData,
  type ProductReleaseDetail,
  type ProductSummary
} from "./api";

type MockReleaseDetail = Omit<ProductReleaseDetail, "requested_version">;

type MockReleaseStore = Record<
  string,
  {
    latest: string;
    versions: Record<string, MockReleaseDetail>;
  }
>;

const productCatalog = productsJson as {
  items: ProductSummary[];
};

const releaseStore = releasesJson as MockReleaseStore;

export function getMockProducts(): ProductSummary[] {
  return productCatalog.items;
}

export function getMockPortfolioHome(): PortfolioHomeData {
  return portfolioHomeJson as PortfolioHomeData;
}

export function getMockReleaseDetail(
  productCode: string,
  version: string,
  channel = defaultReleaseChannel
): ProductReleaseDetail | null {
  const productReleases = releaseStore[productCode];

  if (!productReleases) {
    return null;
  }

  const requestedVersion = version.trim() ? version : "latest";
  const resolvedVersion = requestedVersion === "latest" ? productReleases.latest : requestedVersion;
  const release = productReleases.versions[resolvedVersion] ?? null;
  if (!release) {
    return null;
  }

  if ((release.channel ?? defaultReleaseChannel) !== channel) {
    return null;
  }

  return {
    ...release,
    requested_version: requestedVersion,
    resolved_version: release.resolved_version ?? resolvedVersion,
    channel: release.channel ?? channel
  };
}
