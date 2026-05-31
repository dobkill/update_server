import { type PageBlock, type ProductReleaseDetail } from "./api";

/**
 * 拼接静态资源的基础 URL
 * 后端返回的 download_url 是 ./data/... 格式的相对路径
 * 需要拼接 baseUrl 才能形成完整的下载 URL
 */
function resolveStaticUrl(relativePath: string | undefined | null): string {
  if (!relativePath) {
    return "";
  }
  // 如果已经是完整 URL，直接返回
  if (relativePath.startsWith("http://") || relativePath.startsWith("https://")) {
    return relativePath;
  }
  // 拼接 baseUrl（去除可能的尾部斜杠）
  const baseUrl = window.location.origin.replace(/\/$/, "");
  return baseUrl + relativePath;
}

export function composeReleaseBlocks(release: ProductReleaseDetail): PageBlock[] {
  const pageData = release.page.page_data;

  if (pageData?.blocks?.length) {
    return pageData.blocks;
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