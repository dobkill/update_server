<script setup lang="ts">
import DownloadPanelBlock from "../blocks/DownloadPanelBlock.vue";
import FaqBlock from "../blocks/FaqBlock.vue";
import FeatureGridBlock from "../blocks/FeatureGridBlock.vue";
import FooterCtaBlock from "../blocks/FooterCtaBlock.vue";
import HeroBlock from "../blocks/HeroBlock.vue";
import ImageTextBlock from "../blocks/ImageTextBlock.vue";
import RichTextBlock from "../blocks/RichTextBlock.vue";
import TimelineBlock from "../blocks/TimelineBlock.vue";

type Block = {
  type: string;
  props?: Record<string, unknown>;
};

defineProps<{
  blocks: Block[];
}>();

const componentMap = {
  HeroBlock,
  FeatureGridBlock,
  RichTextBlock,
  ImageTextBlock,
  DownloadPanelBlock,
  TimelineBlock,
  FaqBlock,
  FooterCtaBlock
};
</script>

<template>
  <div class="blocks">
    <component
      :is="componentMap[block.type as keyof typeof componentMap] ?? RichTextBlock"
      v-for="(block, index) in blocks"
      :key="`${block.type}-${index}`"
      v-bind="block.props"
    />
  </div>
</template>

<style scoped>
.blocks {
  display: grid;
  gap: 1.5rem;
  padding: 1.5rem 0 4rem;
}
</style>
