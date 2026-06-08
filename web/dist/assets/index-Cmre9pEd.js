(function(){const t=document.createElement("link").relList;if(t&&t.supports&&t.supports("modulepreload"))return;for(const i of document.querySelectorAll('link[rel="modulepreload"]'))r(i);new MutationObserver(i=>{for(const o of i)if(o.type==="childList")for(const u of o.addedNodes)u.tagName==="LINK"&&u.rel==="modulepreload"&&r(u)}).observe(document,{childList:!0,subtree:!0});function a(i){const o={};return i.integrity&&(o.integrity=i.integrity),i.referrerPolicy&&(o.referrerPolicy=i.referrerPolicy),i.crossOrigin==="use-credentials"?o.credentials="include":i.crossOrigin==="anonymous"?o.credentials="omit":o.credentials="same-origin",o}function r(i){if(i.ep)return;i.ep=!0;const o=a(i);fetch(i.href,o)}})();const T={ownerName:"LIANG Y.",siteName:"Personal Software Lab",subtitle:"Self-built apps, tools, systems, and experiments.",githubUrl:"",email:"",resumeUrl:""};function $(e){const t=e;return!!(t&&typeof t.slug=="string"&&typeof t.name=="string"&&Array.isArray(t.techStack)&&Array.isArray(t.features)&&Array.isArray(t.screenshots)&&t.screenshots.every(a=>a&&typeof a.title=="string"&&typeof a.image=="string"&&(a.desc===void 0||typeof a.desc=="string")))}async function L(e){const t=await fetch(e,{headers:{Accept:"application/json"}}),a=await t.json();if(!t.ok||a.code!==0)throw new Error(a.message||`Request failed: ${t.status}`);return a.data}async function C(){const e=await L("/api/v1/projects");if(!e.profile||!Array.isArray(e.items)||!e.items.every($))throw new Error("项目列表接口返回结构不完整");return{...e,featured:Array.isArray(e.featured)?e.featured.filter($):[],filters:Array.isArray(e.filters)?e.filters:["All"]}}async function U(e){const t=await L(`/api/v1/projects/${encodeURIComponent(e)}`);if(!t.profile||!$(t.project))throw new Error("项目详情接口返回结构不完整");return t}const m="/",D="/projects";function j(e){if(!e||e==="/")return m;const t=e.startsWith("/")?e:`/${e}`;return t.endsWith("/")?t.slice(0,-1):t}function E(e){return(e??"").trim().toLowerCase()}function k(e){const t=j(e);if(t===m)return{page:"catalog"};const a=t.split("/").filter(Boolean).map(r=>decodeURIComponent(r));if(a[0]==="projects"){const r=E(a[1]);return r?{page:"project",slug:r}:{page:"catalog"}}return{page:"catalog"}}function S(){return k(window.location.pathname)}function h(){return m}function p(e){const t=E(e);return t?`${D}/${encodeURIComponent(t)}`:m}function n(e){return String(e??"").replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;").replace(/"/g,"&quot;").replace(/'/g,"&#039;")}function l(e,t){if(!e)return"";const r=e.includes("@")&&!e.startsWith("http")?`mailto:${e}`:e,i=r.startsWith("http");return`
    <a class="nav-link" href="${n(r)}"${i?' target="_blank" rel="noreferrer"':""}>
      ${n(t)}
    </a>
  `}function g(e,t=5){return e.techStack.slice(0,t).map(a=>`<span class="tag">${n(a)}</span>`).join("")}function R(e,t="card"){var a,r;return t==="detail"?e.heroImageUrl||e.coverImageUrl||((a=e.screenshots[0])==null?void 0:a.image)||"":e.coverImageUrl||e.heroImageUrl||((r=e.screenshots[0])==null?void 0:r.image)||""}function f(e,t="card",a="lazy"){const r=R(e,t);return r?`
    <figure class="project-media ${t}">
      <img src="${n(r)}" alt="${n(e.name)} screenshot" loading="${a}" />
    </figure>
  `:`
      <div class="project-media ${t} empty">
        <span>${n(e.category)}</span>
        <strong>${n(e.name)}</strong>
      </div>
    `}function I(e){return e?`
    <a class="hero-preview" href="${p(e.slug)}" data-action="open-project" data-slug="${n(e.slug)}">
      ${f(e,"hero","eager")}
      <div>
        <span>${n(e.category)}</span>
        <strong>${n(e.name)}</strong>
        <p>${n(e.description)}</p>
      </div>
    </a>
  `:""}function F(e,t){return`
    <article class="${t===0?"wide":""}">
      <figure class="screenshot-media">
        <img src="${n(e.image)}" alt="${n(e.title)}" loading="lazy" />
      </figure>
      <h3>${n(e.title)}</h3>
      ${e.desc?`<p>${n(e.desc)}</p>`:""}
    </article>
  `}function N(e,t){return`
    <div class="site-shell">
      <header class="site-header">
        <a class="brand" href="${h()}" data-action="home">${n(t.ownerName)}</a>
        <nav class="site-nav" aria-label="External links">
          ${l(t.githubUrl,"GitHub")}
          ${l(t.email,"Email")}
          ${l(t.resumeUrl,"Resume")}
        </nav>
      </header>
      <main>${e}</main>
    </div>
  `}function O(){return`
    <section class="state-panel">
      <span class="spinner"></span>
      <strong>Loading project lab</strong>
    </section>
  `}function b(e){return`
    <section class="state-panel error">
      <strong>Unable to load</strong>
      <p>${n(e)}</p>
    </section>
  `}function H(e){if(!e.length)return"";const[t,...a]=e;return`
    <section class="section-band" id="featured">
      <div class="section-heading">
        <p>FEATURED PROJECT</p>
        <h2>Built like products, documented like case studies.</h2>
      </div>
      <div class="featured-layout">
        <a class="featured-card" href="${p(t.slug)}" data-action="open-project" data-slug="${n(t.slug)}">
          ${f(t,"large")}
          <div>
            <h3>${n(t.name)}</h3>
            <p>${n(t.description)}</p>
            <div class="tag-row">${g(t)}</div>
          </div>
        </a>
        <div class="featured-stack">
          ${a.slice(0,3).map(r=>`
                <a class="mini-project" href="${p(r.slug)}" data-action="open-project" data-slug="${n(r.slug)}">
                  ${f(r,"small")}
                  <div>
                    <span>${n(r.category)}</span>
                    <h3>${n(r.name)}</h3>
                    <p>${n(r.description)}</p>
                    <div class="tag-row compact">${g(r,3)}</div>
                  </div>
                </a>
              `).join("")}
        </div>
      </div>
    </section>
  `}function W(e,t,a,r){return`
    <section class="section-band" id="projects">
      <div class="catalog-tools">
        <div class="section-heading compact">
          <p>ALL PROJECTS</p>
          <h2>${e.length} projects in the lab</h2>
        </div>
        <div class="project-controls">
          <div class="filter-tabs" role="tablist">
            ${t.map(i=>`
                  <button type="button" class="${i===a?"active":""}" data-action="set-filter" data-filter="${n(i)}">
                    ${n(i)}
                  </button>
                `).join("")}
          </div>
          <input class="project-search" type="search" value="${n(r)}" placeholder="Search projects..." data-action="search-projects" />
        </div>
      </div>
      <div class="project-grid">
        ${e.map(i=>`
              <a class="project-card" href="${p(i.slug)}" data-action="open-project" data-slug="${n(i.slug)}">
                ${f(i)}
                <div class="card-meta">
                  <span>${n(i.category)}</span>
                  <strong>${n(i.status)}</strong>
                </div>
                <h3>${n(i.name)}</h3>
                <p>${n(i.description)}</p>
                <div class="tag-row compact">${g(i,4)}</div>
              </a>
            `).join("")}
      </div>
    </section>
  `}function B(e,t,a,r){const i=e.profile,o=e.featured.length?e.featured:e.items.filter(u=>u.featured);return`
    <section class="hero">
      <div class="hero-copy">
        <span>${n(i.heroLabel)}</span>
        <h1>${n(i.heroTitle)}</h1>
        <p>${n(i.heroDescription)}</p>
        <div class="hero-actions">
          <a class="primary-button" href="#featured">View Featured Projects</a>
          <a class="secondary-button" href="#projects">Browse All Projects</a>
        </div>
      </div>
      ${I(o[0]??e.items[0])}
    </section>
    ${H(o)}
    ${W(t,e.filters,a,r)}
    <footer class="site-footer">
      <span>© 2026 ${n(i.ownerName)}. All rights reserved.</span>
      <nav>
        ${l(i.githubUrl,"GitHub")}
        ${l(i.linkedinUrl,"LinkedIn")}
        ${l(i.twitterUrl,"Twitter")}
        ${l(i.email,"Email")}
      </nav>
      <span>Built with TypeScript & CSS</span>
    </footer>
  `}function q(e){return`
    <dl class="project-meta">
      ${[["Role",e.role],["Type",e.type],["Year",e.year],["Status",e.status],["Platform",e.platform]].map(([a,r])=>`
            <div>
              <dt>${n(a)}</dt>
              <dd>${n(r)}</dd>
            </div>
          `).join("")}
    </dl>
  `}function z(e){return[["Live Demo",e.links.liveDemo],["View on GitHub",e.links.github]].filter(([,a])=>a).map(([a,r],i)=>`
        <a class="${i===0?"primary-button light":"secondary-button light"}" href="${n(r)}" target="_blank" rel="noreferrer">
          ${n(a)}
        </a>
      `).join("")}function M(e,t){return`
    <article class="project-page">
      <header class="project-header">
        <nav class="detail-nav">
          <a href="${h()}" data-action="home">${n(t.ownerName)}</a>
          <a href="${h()}#projects">Back to Projects</a>
        </nav>
        <div class="project-hero">
          <div>
            <span>${e.featured?"FEATURED PROJECT":n(e.category)}</span>
            <h1>${n(e.name)}</h1>
            <p>${n(e.longDescription||e.description)}</p>
            <div class="tag-row detail-tags">${g(e,10)}</div>
            <div class="hero-actions">${z(e)}</div>
          </div>
          ${f(e,"detail","eager")}
        </div>
        ${q(e)}
      </header>

      <section class="case-section overview-grid">
        <div>
          <p class="eyebrow">OVERVIEW</p>
          <h2>${n(e.description)}</h2>
          <p>${n(e.longDescription)}</p>
        </div>
        <div class="feature-grid">
          ${e.features.map(a=>`
                <article>
                  <span></span>
                  <h3>${n(a)}</h3>
                </article>
              `).join("")}
        </div>
      </section>

      <section class="case-section">
        <div class="section-heading light">
          <p>SCREENSHOTS</p>
          <h2>Interface moments</h2>
        </div>
        <div class="screenshot-grid">
          ${e.screenshots.map(F).join("")}
        </div>
      </section>

      <section class="case-section">
        <div class="section-heading light">
          <p>TECH STACK</p>
          <h2>Tools behind the build</h2>
        </div>
        <div class="stack-cloud">${e.techStack.map(a=>`<span>${n(a)}</span>`).join("")}</div>
      </section>

      <section class="case-section">
        <div class="section-heading light">
          <p>ARCHITECTURE</p>
          <h2>System flow</h2>
        </div>
        <div class="architecture-flow">
          ${e.architecture.map((a,r)=>`
                <div>
                  <span>${String(r+1).padStart(2,"0")}</span>
                  <strong>${n(a)}</strong>
                </div>
              `).join("")}
        </div>
      </section>

      <section class="case-section case-cards">
        ${[["Challenge",e.challenge],["Solution",e.solution],["Result",e.result]].map(([a,r])=>`
              <article>
                <h3>${n(a)}</h3>
                <p>${n(r)}</p>
              </article>
            `).join("")}
      </section>
    </article>
  `}const A=document.querySelector("#app");if(!A)throw new Error("Missing #app root element");const d=A,s={currentLocation:S(),projectList:null,projectDetail:null,profile:T,loading:!0,error:"",selectedFilter:"All",searchTerm:""};function G(e){const t=s.searchTerm.trim().toLowerCase();return e.items.filter(a=>{const r=s.selectedFilter==="All"||a.category===s.selectedFilter,i=!t||a.name.toLowerCase().includes(t)||a.description.toLowerCase().includes(t)||a.techStack.some(o=>o.toLowerCase().includes(t));return r&&i})}function V(){return s.currentLocation.page==="project"&&s.projectDetail?`${s.projectDetail.name} | ${s.profile.siteName}`:`${s.profile.siteName} | ${s.profile.subtitle}`}function c(){let e="";s.loading?e=O():s.error?e=b(s.error):s.currentLocation.page==="catalog"&&s.projectList?e=B(s.projectList,G(s.projectList),s.selectedFilter,s.searchTerm):s.currentLocation.page==="project"&&s.projectDetail?e=M(s.projectDetail,s.profile):e=b("页面状态异常，请返回项目列表后重试。"),d.innerHTML=N(e,s.profile),document.title=V()}async function J(){s.loading=!0,s.error="",s.projectDetail=null,c();const e=await C();s.projectList=e,s.profile=e.profile,s.loading=!1,c()}async function x(e){s.loading=!0,s.error="",s.projectDetail=null,c();const t=await U(e);s.profile=t.profile,s.projectDetail=t.project,s.loading=!1,c()}function P(){s.currentLocation=S()}async function w(){if(s.currentLocation.page==="catalog"){await J();return}await x(s.currentLocation.slug)}function y(){const e=window.location.hash.slice(1);if(!e){window.scrollTo({top:0});return}window.setTimeout(()=>{var t;(t=document.getElementById(decodeURIComponent(e)))==null||t.scrollIntoView({behavior:"smooth",block:"start"})},0)}async function v(e){const t=`${window.location.pathname}${window.location.search}${window.location.hash}`;e!==t&&(window.history.pushState({},"",e),P(),await w(),y())}function K(e,t){const a=e.dataset.action;if(!a)return!1;switch(a){case"home":return t.preventDefault(),v(h()),!0;case"open-project":{t.preventDefault();const r=e.dataset.slug;return r&&v(p(r)),!0}case"set-filter":return t.preventDefault(),s.selectedFilter=e.dataset.filter||"All",c(),!0;default:return!1}}function Y(e,t){if(e.target||e.hasAttribute("download"))return;const a=e.getAttribute("href");if(!a||a.startsWith("mailto:")||a.startsWith("tel:"))return;const r=new URL(e.href);if(!(r.origin!==window.location.origin||r.pathname.startsWith("/data/")||r.pathname.startsWith("/api/"))){if(r.pathname===window.location.pathname&&r.hash){t.preventDefault(),`${r.pathname}${r.hash}`!=`${window.location.pathname}${window.location.hash}`&&window.history.pushState({},"",`${r.pathname}${r.hash}`),y();return}t.preventDefault(),v(`${r.pathname}${r.hash}`)}}d.addEventListener("click",e=>{const t=e.target;if(!(t instanceof Element))return;const a=t.closest("[data-action]");if(a&&d.contains(a)&&K(a,e))return;const r=t.closest("a[href]");r&&d.contains(r)&&Y(r,e)});d.addEventListener("input",e=>{const t=e.target;if(!(t instanceof HTMLInputElement)||t.dataset.action!=="search-projects")return;s.searchTerm=t.value,c();const a=d.querySelector('[data-action="search-projects"]');a==null||a.focus()});window.addEventListener("popstate",()=>{P(),w()});c();w().then(y).catch(e=>{s.loading=!1,s.error=e instanceof Error?e.message:String(e),c()});
