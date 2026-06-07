(function(){const t=document.createElement("link").relList;if(t&&t.supports&&t.supports("modulepreload"))return;for(const s of document.querySelectorAll('link[rel="modulepreload"]'))r(s);new MutationObserver(s=>{for(const i of s)if(i.type==="childList")for(const c of i.addedNodes)c.tagName==="LINK"&&c.rel==="modulepreload"&&r(c)}).observe(document,{childList:!0,subtree:!0});function a(s){const i={};return s.integrity&&(i.integrity=s.integrity),s.referrerPolicy&&(i.referrerPolicy=s.referrerPolicy),s.crossOrigin==="use-credentials"?i.credentials="include":s.crossOrigin==="anonymous"?i.credentials="omit":i.credentials="same-origin",i}function r(s){if(s.ep)return;s.ep=!0;const i=a(s);fetch(s.href,i)}})();const v="stable";async function J(){var e;try{const t=await fetch("/api/v1/portfolio-home",{headers:{Accept:"application/json"}});if(!t.ok)return null;const a=await t.json();return a.code!==0||!((e=a.data)!=null&&e.profile)||!Array.isArray(a.data.products)||!Array.isArray(a.data.recommendations)||!Array.isArray(a.data.recent_updates)||!Array.isArray(a.data.future_directions)?null:a.data}catch(t){return console.warn("fetchPortfolioHome fallback",t),null}}async function Y(e,t,a=v){var r,s,i;try{const c=`/api/v1/products/${encodeURIComponent(e)}/Document`,f=new URL(c,window.location.origin);f.searchParams.set("version",t||"latest"),f.searchParams.set("channel",a);const d=await fetch(f.pathname+f.search,{headers:{Accept:"application/json"}});if(!d.ok)return null;const u=await d.json();return u.code!==0||!((r=u.data)!=null&&r.product_code)||!((s=u.data)!=null&&s.requested_version)||!((i=u.data)!=null&&i.resolved_version)?null:u.data}catch(c){return console.warn("fetchReleaseDetail fallback",c),null}}const y="/",b="latest",_="stable";function K(e){if(!e||e==="/")return y;const t=e.startsWith("/")?e:`/${e}`;return t.endsWith("/")?t.slice(0,-1):t}function W(e){return(e??"").trim()}function D(e){return(e??"").trim()||b}function $(e){return(e??"").trim().toLowerCase()||_}function Q(e,t=""){const a=K(e),r=$(new URLSearchParams(t).get("channel"));if(a===y)return{page:"catalog"};const s=a.split("/").filter(Boolean).map(f=>decodeURIComponent(f)),i=W(s[0]),c=D(s[1]);return i?{page:"release",productCode:i,version:c,channel:r}:{page:"catalog"}}function z(){return Q(window.location.pathname,window.location.search)}function ee(){return y}function q(e,t=b,a=_){const r=encodeURIComponent(W(e)),s=D(t);if(!r)return y;const i=s===b?`/${r}`:`/${r}/${encodeURIComponent(s)}`;if($(a)===_)return i;const c=new URL(i,window.location.origin);return c.searchParams.set("channel",$(a)),`${c.pathname}${c.search}`}const te=[{title:"新作品即将加入",status:"准备中",summary:"更多有趣的插件与工具正在构思与开发中。",icon:"+"},{title:"更多插件计划中",status:"计划中",summary:"围绕知识管理与效率提升，持续探索新的想法。",icon:"✦"},{title:"工具类作品准备中",status:"准备中",summary:"轻量、好用的效率工具，正在打磨中。",icon:"⌁"}],ae=[{title:"Obsidian 插件增强",comment:"持续优化记录、任务与知识管理体验。",icon_path:""},{title:"效率工具",comment:"开发更轻量、专注的个人效率工具。",icon_path:""},{title:"Web 实验",comment:"尝试小型 Web 产品与交互实验，探索新的表达方式。",icon_path:""}];function re(e){return typeof e=="object"&&e!==null&&!Array.isArray(e)}function p(e,t=""){return typeof e=="string"?e:typeof e=="number"||typeof e=="boolean"?String(e):t}function l(e,t,a=""){return p(e==null?void 0:e[t],a)}function x(e){return Array.isArray(e)?e.filter(re):[]}function n(e){return p(e).replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;").replace(/"/g,"&quot;").replace(/'/g,"&#39;")}function F(e){return e?e.startsWith("v")?e:`v${e}`:""}function I(e){return e?e.slice(0,10):""}function G(e){return[F(e.latest_version),I(e.published_at||e.updated_at)].filter(Boolean).join(" · ")}function ne(e,t){return e?[F(e.version),I(e.published_at)].filter(Boolean).join(" · "):G(t)}function oe(e){return["◇","◷","</>"][e]??"◇"}function k(e,t){const a=t==="update"?"":` ${t}`;return e.cover_image_url?`
      <div class="cover-frame${a}">
        <img src="${n(e.cover_image_url)}" alt="${n(e.name)}" />
      </div>
    `:`
    <div class="cover-frame${a}">
      <div class="${t==="large"?"daily-mark":"daily-mark compact"}" aria-hidden="true">
        <span></span>
        <strong>Daily</strong>
      </div>
    </div>
  `}function N(e){const t=e.product_code?`<a class="text-action" href="${n(q(e.product_code))}" data-action="open-product" data-product-code="${n(e.product_code)}">查看详情 <span aria-hidden="true">→</span></a>`:"",a=e.github_url?`<a class="text-action" href="${n(e.github_url)}" target="_blank" rel="noreferrer">GitHub <span aria-hidden="true">↗</span></a>`:"";return!t&&!a?"":`<div class="card-actions">${t}${a}</div>`}function w(e){return`
    <div class="chip-row">
      <span>${n(e.status_label||"已发布")}</span>
      <span>${n(e.tag||"Obsidian 插件")}</span>
    </div>
  `}function se(e){return`
    <article class="work-card published">
      ${k(e,"small")}
      <div class="work-copy">
        ${w(e)}
        <h3>${n(e.name)}</h3>
        <p>${n(e.summary)}</p>
        <span class="meta-line">${n(G(e))}</span>
        ${N(e)}
      </div>
    </article>
  `}function ie(e){return`
    <article class="work-card placeholder">
      <div class="placeholder-icon" aria-hidden="true">${n(e.icon)}</div>
      <div class="work-copy">
        <div class="chip-row muted-chip"><span>${n(e.status)}</span></div>
        <h3>${n(e.title)}</h3>
        <p>${n(e.summary)}</p>
      </div>
    </article>
  `}function le(e,t){const a=t.profile.github_url||"",r=t.mobileMenuOpen?" open":"",s=t.mobileMenuOpen?`
      <nav class="mobile-nav" aria-label="移动导航">
        <a class="active" href="/#works" data-action="close-menu">作品</a>
        ${a?`<a href="${n(a)}" target="_blank" rel="noreferrer" data-action="close-menu">GitHub ↗</a>`:""}
      </nav>
    `:"",i=t.currentProductCode||t.currentVersion?`
        <div class="release-context">
          ${t.currentProductCode?`<span>${n(t.currentProductCode)}</span>`:""}
          ${t.currentVersion?`<span>v${n(t.currentVersion.replace(/^v/,""))}</span>`:""}
        </div>
      `:"";return`
    <div class="shell">
      <header class="topbar-wrapper">
        <div class="topbar">
          <button type="button" class="brand" data-action="home">
            <span class="brand-mark" aria-hidden="true"><span></span></span>
            <span class="brand-copy">
              <strong>${n(t.profile.site_name||"YXX Works")}</strong>
              <span>${n(t.profile.subtitle||"产品、插件与创作实验")}</span>
            </span>
          </button>

          <nav class="topbar-nav desktop-nav" aria-label="主导航">
            <a class="active" href="/#works">作品</a>
            ${a?`<a href="${n(a)}" target="_blank" rel="noreferrer">GitHub ↗</a>`:""}
          </nav>

          <button type="button" class="hamburger${r}" aria-label="菜单" aria-expanded="${t.mobileMenuOpen}" data-action="toggle-menu">
            <span></span>
            <span></span>
            <span></span>
          </button>
        </div>
        ${s}
      </header>

      ${i}

      <main class="content">
        ${e}
      </main>
    </div>
  `}function ce(){return`
    <section class="state-card loading-state">
      <div class="spinner" aria-hidden="true"><span></span></div>
      <div class="state-copy">
        <strong>加载中</strong>
        <span>正在读取发布数据...</span>
      </div>
    </section>
  `}function M(e){return`
    <section class="state-card error-state">
      <div class="error-icon" aria-hidden="true">!</div>
      <div class="state-copy">
        <strong>提示</strong>
        <span>${n(e)}</span>
      </div>
    </section>
  `}function de(e){return`<div class="soft-error-bar"><span>${n(e)}</span></div>`}function ue(e){const t=e.recommendations[0]??e.products[0]??null,r=[...e.recent_updates.filter(d=>d.title&&d.description),{product_code:"",name:"",version:"",published_at:"",title:"后续更新",description:"将继续补充插件体验优化、新工具实验和版本记录。"}],s=e.recent_updates[0]??null,i=e.future_directions.length?e.future_directions:ae,c=t?`
      <div class="hero-feature" aria-label="主推项目">
        <div class="ghost-card one"></div>
        <div class="ghost-card two"></div>
        <article class="spotlight-card">
          ${k(t,"large")}
          <div class="spotlight-copy">
            <span class="eyebrow">主推项目</span>
            <h2>${n(t.name)}</h2>
            ${w(t)}
            <p>${n(t.summary)}</p>
          </div>
        </article>
      </div>
    `:"",f=t?`
      <section class="portfolio-section">
        <header class="section-heading"><h2>最近更新</h2></header>
        <div class="updates-panel">
          <article class="update-card">
            ${k(t,"update")}
            <div class="update-copy">
              ${w(t)}
              <h3>${n(t.name)}</h3>
              <p>${n(t.summary)}</p>
              <span class="meta-line">${n(ne(s,t))}</span>
              ${N(t)}
            </div>
          </article>

          <div class="portfolio-timeline" aria-label="更新时间线">
            ${r.map((d,u)=>`
                  <article class="timeline-item${u===0?" active":""}">
                    <span class="timeline-dot" aria-hidden="true"></span>
                    <div>
                      <h3>${n(d.title)}</h3>
                      <p>${n(d.description)}</p>
                    </div>
                  </article>
                `).join("")}
          </div>
        </div>
      </section>
    `:"";return`
    <section class="portfolio-hero">
      <div class="portfolio-hero-copy">
        <h1>个人作品集</h1>
        <p>设计、开发与持续迭代中的作品。</p>
        <div class="hero-actions" aria-label="首页操作">
          <a class="primary-action" href="#works">浏览作品 <span aria-hidden="true">→</span></a>
          ${e.profile.github_url?`<a class="secondary-action" href="${n(e.profile.github_url)}" target="_blank" rel="noreferrer">查看 GitHub <span aria-hidden="true">↗</span></a>`:""}
        </div>
      </div>
      ${c}
    </section>

    ${f}

    <section class="portfolio-section" id="works">
      <header class="section-heading"><h2>全部作品</h2></header>
      <div class="work-grid">
        ${e.products.map(se).join("")}
        ${te.map(ie).join("")}
      </div>
    </section>

    <section class="portfolio-section">
      <header class="section-heading"><h2>未来方向</h2></header>
      <div class="direction-grid">
        ${i.map((d,u)=>`
              <article class="direction-card">
                <span class="direction-icon" aria-hidden="true">${n(oe(u))}</span>
                <h3>${n(d.title)}</h3>
                <p>${n(d.comment)}</p>
              </article>
            `).join("")}
      </div>
    </section>

    <footer class="portfolio-footer">
      <div class="footer-brand">
        <span class="footer-logo" aria-hidden="true"><span></span></span>
        <span>© ${new Date().getFullYear()} ${n(e.profile.site_name||"YXX Works")}</span>
      </div>
      ${e.profile.github_url||e.profile.email?`
            <nav aria-label="站点链接" class="footer-nav">
              ${e.profile.github_url?`<a href="${n(e.profile.github_url)}" target="_blank" rel="noreferrer">GitHub</a>`:""}
              ${e.profile.email?`<a href="mailto:${n(e.profile.email)}">邮件联系</a>`:""}
            </nav>
          `:""}
    </footer>
  `}function pe(e,t){var r;const a=((r=e.published_at)==null?void 0:r.slice(0,10))??"--";return`
    <section class="release-toolbar">
      <button type="button" class="back-link" data-action="back">
        <span aria-hidden="true">←</span> 返回
      </button>

      <div class="release-title">
        <p>${n(e.product_code)}</p>
        <h1>${n(e.title)}</h1>
      </div>

      <dl class="release-meta">
        <div>
          <dt>版本</dt>
          <dd>${n(e.resolved_version)}</dd>
        </div>
        <div>
          <dt>渠道</dt>
          <dd>${n(e.channel)}</dd>
        </div>
        <div>
          <dt>发布</dt>
          <dd>${n(a)}</dd>
        </div>
      </dl>
    </section>

    ${fe(t)}
  `}function fe(e){let t=!1;return`<div class="blocks">${e.map((r,s)=>{const i=r.type==="FeatureGridBlock"&&!t;return t=t||i,me(r,s,i)}).join("")}</div>`}function me(e,t,a){const r=e.props??{};switch(e.type){case"HeroBlock":return he(r);case"FeatureGridBlock":return ge(r,a);case"RichTextBlock":return U(r);case"ImageTextBlock":return ve(r);case"ScreenshotGalleryBlock":return be(r);case"DownloadPanelBlock":return ye(r);case"TimelineBlock":return _e(r);case"FaqBlock":return $e(r);case"FooterCtaBlock":return ke(r);default:return U({title:e.type||`区块 ${t+1}`,content:"当前前端不认识这个区块类型，已按文本区块兜底展示。"})}}function he(e){const t=l(e,"image"),a=l(e,"secondaryText"),r=l(e,"secondaryHref");return`
    <section class="block-hero">
      <div class="block-hero-copy">
        <p class="kicker">${n(l(e,"eyebrow","Release"))}</p>
        <h2>${n(l(e,"title","未配置标题"))}</h2>
        <p>${n(l(e,"subtitle"))}</p>
        <div class="block-actions">
          <a href="${n(l(e,"primaryHref","/"))}">${n(l(e,"primaryText","了解更多"))}</a>
          ${a&&r?`<a class="secondary" href="${n(r)}">${n(a)}</a>`:""}
        </div>
      </div>
      ${t?`<img class="block-hero-image" src="${n(t)}" alt="${n(l(e,"title","发布页图片"))}" />`:""}
    </section>
  `}function ge(e,t){const a=x(e.items);return`
    <section class="block-feature-grid"${t?' id="features"':""}>
      ${a.map(r=>`
            <article class="block-feature-card">
              <h3>${n(p(r.title))}</h3>
              <p>${n(p(r.desc))}</p>
            </article>
          `).join("")}
    </section>
  `}function U(e){return`
    <section class="block-richtext">
      <h3>${n(l(e,"title","说明"))}</h3>
      <p>${n(l(e,"content"))}</p>
    </section>
  `}function ve(e){const t=l(e,"title","图文区块"),a=l(e,"image");return`
    <section class="block-image-text">
      <div class="block-image-copy">
        <h3>${n(t)}</h3>
        <p>${n(l(e,"content"))}</p>
      </div>
      <div class="block-image-art">
        ${a?`<img src="${n(a)}" alt="${n(t)}" />`:'<div class="block-placeholder">可接入发布后的静态资源</div>'}
      </div>
    </section>
  `}function be(e){const t=l(e,"title"),a=l(e,"intro"),r=x(e.items);return`
    <section class="block-gallery">
      <div class="block-gallery-head">
        <h3>${n(t)}</h3>
        ${a?`<p>${n(a)}</p>`:""}
      </div>

      <div class="block-gallery-grid">
        ${r.map(s=>`
              <article class="block-shot-card">
                <div class="block-shot-image-wrap">
                  <img src="${n(p(s.image))}" alt="${n(p(s.title))}" loading="lazy" />
                  <div class="block-shot-overlay"><span>${n(p(s.title))}</span></div>
                </div>
                <div class="block-shot-info">
                  <h4>${n(p(s.title))}</h4>
                  ${p(s.desc)?`<p>${n(p(s.desc))}</p>`:""}
                </div>
              </article>
            `).join("")}
      </div>
    </section>
  `}function ye(e){const t=l(e,"downloadUrl");return`
    <section class="block-download-panel">
      <div class="block-panel-info">
        <p class="tag">当前推荐版本</p>
        <h3>${n(l(e,"version","unknown"))}</h3>
        <p>${n(l(e,"releaseNote"))}</p>
      </div>
      <dl>
        <div>
          <dt>包类型</dt>
          <dd>${n(l(e,"packageType","full"))}</dd>
        </div>
        <div>
          <dt>体积</dt>
          <dd>${n(l(e,"packageSize","0 MB"))}</dd>
        </div>
      </dl>
      ${t?`<a class="download-link" href="${n(t)}">${n(l(e,"downloadText","立即下载"))}</a>`:""}
    </section>
  `}function _e(e){return`
    <section class="block-timeline">
      ${(Array.isArray(e.items)?e.items:[]).map((a,r)=>`
            <div class="block-timeline-row">
              <span class="step-num">${r+1}</span>
              <p>${n(a)}</p>
            </div>
          `).join("")}
    </section>
  `}function $e(e){return`
    <section class="block-faq">
      ${x(e.items).map(a=>`
            <article class="block-faq-item">
              <h3>${n(p(a.question))}</h3>
              <p>${n(p(a.answer))}</p>
            </article>
          `).join("")}
    </section>
  `}function ke(e){return`
    <section class="block-footer-cta">
      <div class="block-cta-copy">
        <h3>${n(l(e,"title","继续扩展页面能力"))}</h3>
        <p>${n(l(e,"desc"))}</p>
      </div>
      <a class="block-cta-button" href="${n(l(e,"actionHref","/"))}">${n(l(e,"actionText","前往"))}</a>
    </section>
  `}const we={site_name:"YXX Works",subtitle:"产品、插件与创作实验",github_url:"",email:""},xe=[{product_code:"Daily",name:"Daily",summary:"用于管理每日任务、快速记录和回顾每日进度。",cover_image_url:"",github_url:"",latest_version:"v1.0.0",published_at:"2026-06-05T09:00:00Z",tag:"Obsidian 插件",status_label:"已发布"}],Be=[{product_code:"Daily",name:"Daily",summary:"用于管理每日任务、快速记录和回顾每日进度。",cover_image_url:"",github_url:"",latest_version:"v1.0.0",updated_at:"2026-06-05T09:00:00Z",tag:"Obsidian 插件",status_label:"已发布"}],Ce=[{product_code:"Daily",name:"Daily",version:"v1.0.0",published_at:"2026-06-05T09:00:00Z",title:"2026-06-05 · Daily v1.0.0 发布",description:"完成首个公开版本，用于每日任务、快速记录和进度回顾。"}],Te=[{title:"Obsidian 插件增强",comment:"持续优化记录、任务与知识管理体验。",icon_path:""},{title:"效率工具",comment:"开发更轻量、专注的个人效率工具。",icon_path:""},{title:"Web 实验",comment:"尝试小型 Web 产品与交互实验，探索新的表达方式。",icon_path:""}],Se={profile:we,recommendations:xe,products:Be,recent_updates:Ce,future_directions:Te},Le={latest:"v1.0.0",versions:{"v1.0.0":{product_code:"Daily",resolved_version:"v1.0.0",channel:"stable",title:"Daily v1.0.0",published_at:"2026-06-05T09:00:00Z",release_notes_summary:"Daily v1.0.0 首个可用版本，包含每日任务、项目规划和进度复盘页面。",page:{html_entry_url:"/data/releases/Daily/v1.0.0/vue/DailyReleasePage.html",page_data:{blocks:[{type:"HeroBlock",props:{eyebrow:"Obsidian Plugin",title:"Daily",subtitle:"任务管理 · 项目规划 · 进度复盘 —— 在 Obsidian 里打通每一天的工作流。支持表格、看板、甘特图、思维导图四种项目视图，配合热力图和趋势图复盘每日进展。",primaryText:"下载 v1.0.0",primaryHref:"/data/packages/Daily/v1.0.0/Daily-v1.0.0.zip",secondaryText:"查看功能详情",secondaryHref:"#features",image:"/data/releases/Daily/v1.0.0/vue/总览.png"}},{type:"FeatureGridBlock",props:{items:[{title:"任务总览",desc:"今日任务、时间线、30天完成趋势、周任务图、年度热力图，多维度掌握任务节奏。"},{title:"项目进度",desc:"创建项目、管理任务、导出数据。支持项目导入，一站式管理所有项目。"},{title:"快速记录",desc:"写日记、追加笔记、创建任务、导图补充，覆盖不同记录场景。"},{title:"多视图切换",desc:"同一项目可在表格、看板、甘特图和思维导图之间自由切换。"}]}},{type:"RichTextBlock",props:{title:"适合谁使用",content:"Daily 面向在 Obsidian 中管理个人项目和每日任务的人。无论备考复习、写作计划还是日常习惯追踪，它都能把零散的记录沉淀为可追踪的任务与项目视图。"}},{type:"ScreenshotGalleryBlock",props:{title:"总览与数据看板",intro:"总览页面提供两种视角：任务总览展示今日任务和各项趋势图；项目进度集中管理所有项目的创建与跟踪。",items:[{title:"总览页面",desc:"任务总览和项目进度两个 Tab，一屏掌握全局。",image:"/data/releases/Daily/v1.0.0/vue/总览.png"},{title:"年度热力图",desc:"类似 GitHub 贡献图的年度活跃热力图，直观展示持续节奏。",image:"/data/releases/Daily/v1.0.0/vue/项目总览-热力图.png"},{title:"项目进度页",desc:"集中查看和管理所有项目的整体进度。",image:"/data/releases/Daily/v1.0.0/vue/项目进度页.png"}]}},{type:"DownloadPanelBlock",props:{version:"v1.0.0",packageType:"obsidian-plugin",packageSize:"92.5 KB",releaseNote:"Daily v1.0.0 首个可用版本，包含每日任务、项目规划和进度复盘页面。",downloadText:"下载 Daily v1.0.0",downloadUrl:"/data/packages/Daily/v1.0.0/Daily-v1.0.0.zip"}},{type:"FaqBlock",props:{items:[{question:"如何安装？",answer:"下载 zip 后解压，将 main.js、manifest.json 和 styles.css 放入 Obsidian vault 的 .obsidian/plugins/Daily 目录，然后在社区插件里启用。"},{question:"支持哪些项目视图？",answer:"每个项目支持表格、看板、甘特图和思维导图四种视图。"}]}},{type:"FooterCtaBlock",props:{title:"Daily v1.0.0",desc:"每日任务、项目规划和进度复盘已经可以作为一个插件版本发布。",actionText:"返回列表",actionHref:"/"}}]}}}}},Pe={Daily:Le,"project-manage":{latest:"2.3.1",versions:{"2.3.1":{product_code:"project-manage",resolved_version:"2.3.1",channel:"stable",title:"Project Manage 2.3.1",published_at:"2026-05-23T10:00:00Z",release_notes_summary:"修复导出卡死问题，优化首页加载速度，并补充了最新版本的统一下载入口。",page:{html_entry_url:"/public/releases/project-manage/2.3.1/index.html",page_data:{hero:{title:"Project Manage 2.3.1",subtitle:"本次版本聚焦导出稳定性、首页首屏速度和下载链路统一化。"},summary:{title:"版本摘要",content:"页面详情由本地已发布 JSON 组装而成。当前实现里，web 会把产品参数发送给 API；如果接口不可用，则回退到这份本地版本数据继续渲染。"},highlights:{items:[{title:"导出稳定",desc:"处理批量导出时的锁竞争问题，降低高并发下的失败率。"},{title:"首屏更快",desc:"首页数据加载链路压缩，避免重复请求导致的首屏抖动。"},{title:"下载统一",desc:"详情页下载区块直接使用后端返回的 ./data/... 路径，前端拼接 baseUrl 后进行下载。"}]},timeline:{items:["读取版本详情 JSON","组装 Hero、摘要、特性、下载和 FAQ 区块","渲染出当前版本详情页","用户点击下载后通过 baseUrl + download_url 拼接完整 URL 进行下载"]},download:{version:"2.3.1",package_type:"full",package_size:"89 MB",release_note:"推荐 Windows x64 Stable 安装包。",download_text:"下载 Windows x64 Stable",download_url:"./data/packages/project-manage/2.3.1/installer.exe",platform:"windows",arch:"x64",channel:"stable"},faq:{items:[{question:"这个详情页的数据从哪里来？",answer:"正式场景由 `/api/v1/products/{product_code}/Document?version=...` 返回，当前代码内置了同结构本地 JSON 作为后备。"},{question:"不传版本号时会展示哪个版本？",answer:"访问 `/{product_code}` 时，web 默认请求 `Document?version=latest`，由 API 解析成当前最新版本。"}]},footer_cta:{title:"继续查看其他小项目",desc:"返回产品列表页，可以继续搜索和切换到其他小项目详情。",action_text:"返回产品列表",action_href:"/"}}}},"2.3.0":{product_code:"project-manage",resolved_version:"2.3.0",channel:"stable",title:"Project Manage 2.3.0",published_at:"2026-04-28T09:00:00Z",release_notes_summary:"补齐版本详情页结构，为下载入口和发布说明建立统一的数据模型。",page:{html_entry_url:"/public/releases/project-manage/2.3.0/index.html",page_data:{hero:{title:"Project Manage 2.3.0",subtitle:"首次将发布页切换到数据驱动模式。"},summary:{title:"版本摘要",content:"2.3.0 版本完成了发布页字段收敛，让页面内容和存储 JSON 的边界清晰下来。"},download:{version:"2.3.0",package_type:"full",package_size:"84 MB",release_note:"历史版本下载入口。",download_text:"下载 2.3.0",download_url:"./data/packages/project-manage/2.3.0/installer.exe",platform:"windows",arch:"x64",channel:"stable"},footer_cta:{title:"返回产品列表",desc:"你也可以直接访问 `/project-manage` 查看最新版本。",action_text:"查看全部产品",action_href:"/"}}}}}},"smart-ops":{latest:"1.8.4",versions:{"1.8.4":{product_code:"smart-ops",resolved_version:"1.8.4",channel:"stable",title:"Smart Ops 1.8.4",published_at:"2026-05-19T08:30:00Z",release_notes_summary:"增强巡检结果回执说明和下载页可读性。",page:{html_entry_url:"/public/releases/smart-ops/1.8.4/index.html",page_data:{hero:{title:"Smart Ops 1.8.4",subtitle:"运维助手版本页改为统一页面装配流程。"},summary:{title:"版本摘要",content:"该页面展示了同一套 web 运行时如何复用到另一个小项目。"},download:{version:"1.8.4",package_type:"full",package_size:"76 MB",release_note:"推荐 Linux x64 Stable 安装包。",download_text:"下载 Linux x64 Stable",download_url:"./data/packages/smart-ops/1.8.4/installer.tar.gz",platform:"linux",arch:"x64",channel:"stable"},footer_cta:{title:"继续浏览其他项目",desc:"产品列表页支持按项目名称、编码和简介检索。",action_text:"回到列表页",action_href:"/"}}}}}},"data-bridge":{latest:"3.1.0",versions:{"3.1.0":{product_code:"data-bridge",resolved_version:"3.1.0",channel:"stable",title:"Data Bridge 3.1.0",published_at:"2026-05-11T14:20:00Z",release_notes_summary:"统一转换结果出口和发布页下载文案。",page:{html_entry_url:"/public/releases/data-bridge/3.1.0/index.html",page_data:{hero:{title:"Data Bridge 3.1.0",subtitle:"数据桥接项目的最新版本说明和下载入口。"},summary:{title:"版本摘要",content:"详情页需要的说明、发布时间和下载参数都可以提前保存在本地 JSON 中。"},download:{version:"3.1.0",package_type:"full",package_size:"63 MB",release_note:"推荐 MacOS arm64 Stable 安装包。",download_text:"下载 MacOS arm64 Stable",download_url:"./data/packages/data-bridge/3.1.0/installer.dmg",platform:"macos",arch:"arm64",channel:"stable"},footer_cta:{title:"返回产品列表",desc:"继续搜索其他小项目，或直接切换到指定版本路径。",action_text:"查看全部产品",action_href:"/"}}}}}}},He=Pe;function V(){return Se}function Oe(e,t,a=v){const r=He[e];if(!r)return null;const s=t.trim()?t:"latest",i=s==="latest"?r.latest:s,c=r.versions[i]??null;return!c||(c.channel??v)!==a?null:{...c,requested_version:s,resolved_version:c.resolved_version??i,channel:c.channel??a}}function h(e){if(!e)return"";if(e.startsWith("http://")||e.startsWith("https://"))return e;const t=window.location.origin.replace(/\/$/,"");if(e.startsWith("/"))return`${t}${e}`;const a=e.startsWith("./")?e.slice(2):e;return`${t}/${a}`}function Ae(e){const t={...e.props??{}};return typeof t.image=="string"&&(t.image=h(t.image)),typeof t.downloadUrl=="string"&&(t.downloadUrl=h(t.downloadUrl)),Array.isArray(t.items)&&(t.items=t.items.map(a=>{if(!a||typeof a!="object")return a;const r={...a};return typeof r.image=="string"&&(r.image=h(r.image)),r})),{...e,props:t}}function je(e){var i,c,f,d,u,T,S,L,P,H,O,A,j,R;const t=e.page.page_data;if((i=t==null?void 0:t.blocks)!=null&&i.length)return t.blocks.map(Ae);const a=[];a.push({type:"HeroBlock",props:{title:((c=t==null?void 0:t.hero)==null?void 0:c.title)??e.title,subtitle:((f=t==null?void 0:t.hero)==null?void 0:f.subtitle)??((d=t==null?void 0:t.summary)==null?void 0:d.content)??e.release_notes_summary,primaryText:((u=t==null?void 0:t.hero)==null?void 0:u.primary_text)??"返回产品列表",primaryHref:((T=t==null?void 0:t.hero)==null?void 0:T.primary_href)??"/"}});const r=((S=t==null?void 0:t.summary)==null?void 0:S.content)??e.release_notes_summary;r&&a.push({type:"RichTextBlock",props:{title:((L=t==null?void 0:t.summary)==null?void 0:L.title)??"版本摘要",content:r}}),(H=(P=t==null?void 0:t.highlights)==null?void 0:P.items)!=null&&H.length&&a.push({type:"FeatureGridBlock",props:{items:t.highlights.items}}),(A=(O=t==null?void 0:t.timeline)==null?void 0:O.items)!=null&&A.length&&a.push({type:"TimelineBlock",props:{items:t.timeline.items}});const s=t==null?void 0:t.download;return s&&a.push({type:"DownloadPanelBlock",props:{version:s.version??e.resolved_version,packageType:s.package_type??"full",packageSize:s.package_size??"待接口补充",releaseNote:s.release_note??e.release_notes_summary,downloadText:s.download_text??"下载当前版本",downloadUrl:h(s.download_url)}}),(R=(j=t==null?void 0:t.faq)==null?void 0:j.items)!=null&&R.length&&a.push({type:"FaqBlock",props:{items:t.faq.items}}),t!=null&&t.footer_cta&&a.push({type:"FooterCtaBlock",props:{title:t.footer_cta.title??"返回产品列表",desc:t.footer_cta.desc??"",actionText:t.footer_cta.action_text??"查看全部产品",actionHref:t.footer_cta.action_href??"/"}}),a}const E=document.querySelector("#app");if(!E)throw new Error("Missing #app root element");const g=E,Re={site_name:"YXX Works",subtitle:"产品、插件与创作实验",github_url:"",email:""},o={currentLocation:z(),portfolioHome:V(),release:null,loading:!0,error:"",mobileMenuOpen:!1};function Me(){if(o.currentLocation.page==="catalog"){const e=o.portfolioHome.profile;return`${e.site_name||"YXX Works"} | ${e.subtitle||"产品、插件与创作实验"}`}return o.release?`${o.release.title} | ${o.release.product_code}`:"版本详情 | 更新平台"}function Ue(){var e;return((e=o.portfolioHome)==null?void 0:e.profile)??Re}function We(){return o.currentLocation.page==="release"?o.currentLocation.productCode:""}function De(){return o.currentLocation.page!=="release"?"":o.release?o.release.resolved_version:o.currentLocation.version}function ze(){return o.currentLocation.page==="release"?o.currentLocation.channel:v}function m(){let e="";o.loading?e=ce():o.error&&!o.release?e=M(o.error):o.currentLocation.page==="catalog"?e=ue(o.portfolioHome):o.release?e=pe(o.release,je(o.release)):e=M("页面状态异常，请返回产品列表后重试。"),g.innerHTML=le(e,{currentProductCode:We(),currentVersion:De(),mobileMenuOpen:o.mobileMenuOpen,profile:Ue()})+(o.error&&o.release?de(o.error):""),document.title=Me()}async function qe(){o.loading=!0,o.error="",o.release=null,m();const e=await J();o.portfolioHome=e??V(),o.loading=!1,m()}async function Fe(e,t,a){o.loading=!0,o.error="",m();const r=await Y(e,t,a);if(r){o.release=r,o.loading=!1,m();return}const s=Oe(e,t,a);s?(o.release=s,o.error=`接口暂不可用，当前显示 ${a} 渠道的本地示例数据。`):(o.release=null,o.error=`未找到 ${e} ${t} 的 ${a} 渠道数据。`),o.loading=!1,m()}function X(){o.currentLocation=z(),o.mobileMenuOpen=!1}async function B(){if(o.currentLocation.page==="catalog"){await qe();return}await Fe(o.currentLocation.productCode,o.currentLocation.version,o.currentLocation.channel)}function Z(){const e=window.location.hash.slice(1);if(!e){window.scrollTo({top:0});return}window.setTimeout(()=>{var t;(t=document.getElementById(decodeURIComponent(e)))==null||t.scrollIntoView({behavior:"smooth",block:"start"})},0)}async function C(e){const t=`${window.location.pathname}${window.location.search}${window.location.hash}`;e!==t&&(window.history.pushState({},"",e),X(),await B(),Z())}function Ie(){C(ee())}function Ge(e){C(q(e,b,ze()))}function Ne(e,t){const a=e.dataset.action;if(!a)return!1;switch(a){case"home":case"back":return t.preventDefault(),Ie(),!0;case"toggle-menu":return t.preventDefault(),o.mobileMenuOpen=!o.mobileMenuOpen,m(),!0;case"close-menu":return o.mobileMenuOpen=!1,!1;case"open-product":{t.preventDefault();const r=e.dataset.productCode;return r&&Ge(r),!0}default:return!1}}function Ve(e,t){if(e.target||e.hasAttribute("download"))return;const a=e.getAttribute("href");if(!a||a.startsWith("mailto:")||a.startsWith("tel:"))return;const r=new URL(e.href);if(!(r.origin!==window.location.origin||r.pathname.startsWith("/data/")||r.pathname.startsWith("/api/"))){if(r.pathname===window.location.pathname&&r.search===window.location.search&&r.hash){t.preventDefault(),o.mobileMenuOpen=!1,`${r.pathname}${r.search}${r.hash}`!=`${window.location.pathname}${window.location.search}${window.location.hash}`&&window.history.pushState({},"",`${r.pathname}${r.search}${r.hash}`),m(),Z();return}t.preventDefault(),C(`${r.pathname}${r.search}${r.hash}`)}}g.addEventListener("click",e=>{const t=e.target;if(!(t instanceof Element))return;const a=t.closest("[data-action]");if(a&&g.contains(a)&&Ne(a,e))return;const r=t.closest("a[href]");r&&g.contains(r)&&Ve(r,e)});window.addEventListener("popstate",()=>{X(),B()});m();B().catch(e=>{o.loading=!1,o.error=`页面加载失败：${e instanceof Error?e.message:String(e)}`,m()});
