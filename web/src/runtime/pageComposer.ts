import { type PageBlock, type ProductReleaseDetail } from "./api";

export function resolveStaticUrl(relativePath: string | undefined | null): string {
  if (!relativePath) {
    return "";
  }

  if (relativePath.startsWith("http://") || relativePath.startsWith("https://")) {
    return relativePath;
  }

  const baseUrl = window.location.origin.replace(/\/$/, "");
  if (relativePath.startsWith("/")) {
    return `${baseUrl}${relativePath}`;
  }

  const normalizedPath = relativePath.startsWith("./")
    ? relativePath.slice(2)
    : relativePath;
  return `${baseUrl}/${normalizedPath}`;
}

function resolveBlockAssets(block: PageBlock): PageBlock {
  const props = { ...(block.props ?? {}) };

  if (typeof props.image === "string") {
    props.image = resolveStaticUrl(props.image);
  }

  if (typeof props.downloadUrl === "string") {
    props.downloadUrl = resolveStaticUrl(props.downloadUrl);
  }

  if (Array.isArray(props.items)) {
    props.items = props.items.map((item) => {
      if (!item || typeof item !== "object") {
        return item;
      }

      const nextItem = { ...(item as Record<string, unknown>) };
      if (typeof nextItem.image === "string") {
        nextItem.image = resolveStaticUrl(nextItem.image);
      }
      return nextItem;
    });
  }

  return {
    ...block,
    props
  };
}

export function composeReleaseBlocks(release: ProductReleaseDetail): PageBlock[] {
  const pageData = release.page.page_data;

  if (pageData?.blocks?.length) {
    return pageData.blocks.map(resolveBlockAssets);
  }

  const blocks: PageBlock[] = [];

  blocks.push({
    type: "HeroBlock",
    props: {
      title: pageData?.hero?.title ?? release.title,
      subtitle:
        pageData?.hero?.subtitle ??
        pageData?.summary?.content ??
        release.release_notes_summary,
      primaryText: pageData?.hero?.primary_text ?? "返回产品列表",
      primaryHref: pageData?.hero?.primary_href ?? "/"
    }
  });

  const summaryContent = pageData?.summary?.content ?? release.release_notes_summary;
  if (summaryContent) {
    blocks.push({
      type: "RichTextBlock",
      props: {
        title: pageData?.summary?.title ?? "版本摘要",
        content: summaryContent
      }
    });
  }

  if (pageData?.highlights?.items?.length) {
    blocks.push({
      type: "FeatureGridBlock",
      props: {
        items: pageData.highlights.items
      }
    });
  }

  if (pageData?.timeline?.items?.length) {
    blocks.push({
      type: "TimelineBlock",
      props: {
        items: pageData.timeline.items
      }
    });
  }

  const download = pageData?.download;
  if (download) {
    blocks.push({
      type: "DownloadPanelBlock",
      props: {
        version: download.version ?? release.resolved_version,
        packageType: download.package_type ?? "full",
        packageSize: download.package_size ?? "待接口补充",
        releaseNote: download.release_note ?? release.release_notes_summary,
        downloadText: download.download_text ?? "下载当前版本",
        downloadUrl: resolveStaticUrl(download.download_url)
      }
    });
  }

  if (pageData?.faq?.items?.length) {
    blocks.push({
      type: "FaqBlock",
      props: {
        items: pageData.faq.items
      }
    });
  }

  if (pageData?.footer_cta) {
    blocks.push({
      type: "FooterCtaBlock",
      props: {
        title: pageData.footer_cta.title ?? "返回产品列表",
        desc: pageData.footer_cta.desc ?? "",
        actionText: pageData.footer_cta.action_text ?? "查看全部产品",
        actionHref: pageData.footer_cta.action_href ?? "/"
      }
    });
  }

  return blocks;
}
