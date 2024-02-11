#pragma once

inline constexpr char html_boxplot_template[] = R"TEMPLATE(
{
    x: [{{#result}}{{context(length)}},{{/result}}],
    y: [{{#result}}{{average(elapsed)}},{{/result}}],
},
)TEMPLATE";
