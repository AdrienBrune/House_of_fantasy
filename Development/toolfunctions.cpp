#include "toolfunctions.h"

// Internal helper: builds a smooth closed blob path with organic irregular edges.
// numPts control points are placed around (cx, cy) at radii that randomly vary
// between (1 - irregularity) and (1 + irregularity) times the base radii.
// Cubic bezier interpolation ensures smooth (not polygon-shaped) boundaries.
static QPainterPath irregularBlob(double cx, double cy, double rx, double ry,
                                   int numPts, QRandomGenerator& rng,
                                   float irregularity = 0.42f)
{
    QVector<QPointF> pts;
    pts.reserve(numPts);
    for (int i = 0; i < numPts; i++) {
        double angle = 2.0 * M_PI * i / numPts;
        double v = 1.0 - irregularity + (rng.bounded(1000u) / 1000.0) * irregularity * 2.0;
        v = qMax(0.25, v);
        pts.append(QPointF(cx + rx * v * qCos(angle), cy + ry * v * qSin(angle)));
    }
    QPainterPath path;
    path.moveTo(pts[0]);
    for (int i = 0; i < numPts; i++) {
        int prev  = (i - 1 + numPts) % numPts;
        int next  = (i + 1) % numPts;
        int next2 = (i + 2) % numPts;
        QPointF c1 = pts[i]    + (pts[next]  - pts[prev])  * 0.25;
        QPointF c2 = pts[next] - (pts[next2] - pts[i])     * 0.25;
        path.cubicTo(c1, c2, pts[next]);
    }
    path.closeSubpath();
    return path;
}

QPixmap ToolFunctions::generateGrassTile(int size, quint32 seed)
{
    QPixmap tile(size, size);
    QPainter p(&tile);
    p.setRenderHint(QPainter::Antialiasing, false);

    // Base layer uses a FIXED seed shared by all variants — this guarantees
    // that every variant has the same seamless base texture, so adjacent tiles
    // of different variants never show a visible seam at their boundary.
    QRandomGenerator baseRng(0xA5F3C2E1u);

    // Decoration seed is variant-specific (each tile looks different on top).
    QRandomGenerator rng(seed);

    // base
    p.fillRect(0, 0, size, size, QColor(62, 112, 32));

    // color patches — identical for all variants, seamless with any neighbor
    p.setPen(Qt::NoPen);
    const QColor patches[] = {
        QColor(42,  88, 18, 65),   // dark
        QColor(95, 148, 48, 55),   // bright
        QColor(55, 105, 28, 50),   // neutral
        QColor(78, 130, 38, 45),   // medium green
    };
    for (int i = 0; i < 160; i++)
    {
        int cx = static_cast<int>(baseRng.bounded(static_cast<quint32>(size)));
        int cy = static_cast<int>(baseRng.bounded(static_cast<quint32>(size)));
        int rx = 12 + static_cast<int>(baseRng.bounded(40u));
        int ry = 10 + static_cast<int>(baseRng.bounded(30u));
        QColor c = patches[baseRng.bounded(4u)];
        p.setBrush(c);
        for (int ox : {-size, 0, size})
            for (int oy : {-size, 0, size})
                p.drawEllipse(cx - rx/2 + ox, cy - ry/2 + oy, rx, ry);
    }

    // grass blades — identical for all variants
    const int bladeCount = (size * size) / 28;
    for (int i = 0; i < bladeCount; i++) {
        int x = static_cast<int>(baseRng.bounded(static_cast<quint32>(size)));
        int y = static_cast<int>(baseRng.bounded(static_cast<quint32>(size)));
        int len  = 3 + static_cast<int>(baseRng.bounded(5u));
        int tilt = static_cast<int>(baseRng.bounded(5u)) - 2;
        QColor blade(
            42 + static_cast<int>(baseRng.bounded(35u)),
            88 + static_cast<int>(baseRng.bounded(55u)) - 20,
            12 + static_cast<int>(baseRng.bounded(18u))
        );
        p.setPen(QPen(blade, 1));
        for (int ox : {-size, 0, size})
            for (int oy : {-size, 0, size})
                p.drawLine(x + ox, y + oy, x + tilt + ox, y - len + oy);
    }

    // ── Decorative tile elements (variant-specific, drawn on top) ─────────────
    // Each variant gets different decorations via rng(seed).
    // All drawn at 9 offset positions for seamless self-tiling.

    // Helper: draw a rock (shadow + body + highlight) at 9 positions
    auto drawRock = [&](int cx, int cy, int rw, int rh, int a,
                        QColor shadow, QColor body, QColor hl) {
        p.setPen(Qt::NoPen);
        for (int ox : {-size, 0, size}) {
            for (int oy : {-size, 0, size}) {
                p.setBrush(shadow);
                p.drawEllipse(cx-rw/2+ox,   cy-rh/2+1+oy, rw,       rh);
                p.setBrush(body);
                p.drawEllipse(cx-rw/2+ox,   cy-rh/2+oy,   rw,       rh);
                p.setBrush(QColor(hl.red(), hl.green(), hl.blue(), a/2));
                p.drawEllipse(cx-rw/3+ox,   cy-rh/2+oy,   rw*2/3,   rh/2);
            }
        }
    };

    // Helper: draw a grass tuft cluster at 9 positions
    auto drawGrassTuft = [&](int tx, int ty, int n, int lenMin, int lenMax,
                              int cr, int cg, int cb, int spread) {
        for (int s = 0; s < n; s++) {
            int sx   = tx + static_cast<int>(rng.bounded(static_cast<quint32>(spread * 2))) - spread;
            int sy   = ty + static_cast<int>(rng.bounded(static_cast<quint32>(spread))) - spread / 2;
            int len  = lenMin + static_cast<int>(rng.bounded(static_cast<quint32>(lenMax - lenMin + 1)));
            int tilt = static_cast<int>(rng.bounded(9u)) - 4;
            int v    = static_cast<int>(rng.bounded(28u)) - 14;
            QColor c(qBound(0,cr+v,255), qBound(0,cg+v,255), qBound(0,cb+v,255));
            p.setPen(QPen(c, 1));
            for (int ox : {-size, 0, size})
                for (int oy : {-size, 0, size})
                    p.drawLine(sx+ox, sy+oy, sx+tilt+ox, sy-len+oy);
        }
    };

    // Margin to keep grass tuft centers away from tile edges.
    // Blades grow upward by up to lenMax=18px, spread adds ~12px radially,
    // so 35px margin on all sides prevents any blade from being clipped.
    const int tM = 35;
    const int tRange = size - 2 * tM;

    // 1. Tall yellow-green grass tufts — always 1-3 clusters per tile
    {
        int clusters = 1 + static_cast<int>(rng.bounded(3u));
        for (int cl = 0; cl < clusters; cl++) {
            int tx = tM + static_cast<int>(rng.bounded(static_cast<quint32>(tRange)));
            int ty = tM + static_cast<int>(rng.bounded(static_cast<quint32>(tRange)));
            int n  = 8 + static_cast<int>(rng.bounded(8u));
            drawGrassTuft(tx, ty, n, 11, 18, 90, 165, 35, 12);
        }
    }

    // 2. Aqua/teal tall grass — 1 in 2 tiles, 1-2 clusters
    if (rng.bounded(2u) == 0) {
        int clusters = 1 + static_cast<int>(rng.bounded(2u));
        for (int cl = 0; cl < clusters; cl++) {
            int tx = tM + static_cast<int>(rng.bounded(static_cast<quint32>(tRange)));
            int ty = tM + static_cast<int>(rng.bounded(static_cast<quint32>(tRange)));
            int n  = 7 + static_cast<int>(rng.bounded(7u));
            drawGrassTuft(tx, ty, n, 10, 17, 100, 150, 40, 12);
        }
    }

    // 4. Big rocks — 1 in 3 tiles, 3 distinct rock colors, 1-3 rocks per cluster
    if (rng.bounded(5u) == 0) {
        int cx   = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int cy   = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int n    = 1 + static_cast<int>(rng.bounded(3u));
        int type = static_cast<int>(rng.bounded(3u));
        for (int i = 0; i < n; i++) {
            int px = cx + static_cast<int>(rng.bounded(30u)) - 15;
            int py = cy + static_cast<int>(rng.bounded(22u)) - 11;
            int rw = 12 + static_cast<int>(rng.bounded(12u));
            int rh =  8 + static_cast<int>(rng.bounded(static_cast<quint32>(rw / 2)));
            int a  = 215 + static_cast<int>(rng.bounded(35u));
            int v  = static_cast<int>(rng.bounded(25u)) - 12;
            if (type == 0) // dark charcoal grey
                drawRock(px,py,rw,rh,a, QColor(35+v,30+v,25+v,a), QColor(75+v,67+v,58+v,a), QColor(118+v,108+v,94+v,255));
            else if (type == 1) // warm sandstone
                drawRock(px,py,rw,rh,a, QColor(110+v,110+v,110+v,a), QColor(160+v,160+v,160+v,a), QColor(200+v,200+v,200+v,255));
            else // mossy green rock
                drawRock(px,py,rw,rh,a, QColor(42+v,58+v,28+v,a), QColor(74+v,98+v,48+v,a), QColor(115+v,142+v,80+v,255));
        }
    }

    // 5. Dead leaves cluster — 1 in 3 tiles, 6-13 leaves, more opaque
    if (rng.bounded(7u) == 0) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int n  = 6 + static_cast<int>(rng.bounded(8u));
        const QColor leaves[] = {
            QColor(148,85,28), QColor(185,108,20), QColor(168,138,30),
            QColor(115,40,16), QColor(152,92,18),  QColor(130,112,26),
        };
        p.setPen(Qt::NoPen);
        for (int i = 0; i < n; i++) {
            int lx = cx + static_cast<int>(rng.bounded(34u)) - 17;
            int ly = cy + static_cast<int>(rng.bounded(24u)) - 12;
            int rw = 5 + static_cast<int>(rng.bounded(11u));
            int rh = 3 + static_cast<int>(rng.bounded(static_cast<quint32>(rw / 2)));
            QColor c = leaves[rng.bounded(6u)];
            p.setBrush(QColor(c.red(), c.green(), c.blue(), 145 + static_cast<int>(rng.bounded(90u))));
            for (int ox : {-size, 0, size})
                for (int oy : {-size, 0, size})
                    p.drawEllipse(lx-rw/2+ox, ly-rh/2+oy, rw, rh);
        }
    }

    // 7. Small dirt spot — 1 in 3 tiles, 4-8 blobs, more opaque
    if (rng.bounded(5u) == 0) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int n  = 4 + static_cast<int>(rng.bounded(5u));
        p.setPen(Qt::NoPen);
        for (int i = 0; i < n; i++) {
            int dx = cx + static_cast<int>(rng.bounded(24u)) - 12;
            int dy = cy + static_cast<int>(rng.bounded(18u)) - 9;
            int r  = 5 + static_cast<int>(rng.bounded(10u));
            int v  = static_cast<int>(rng.bounded(16u)) - 8;
            p.setBrush(QColor(72+v, 48+v, 18+v, 105 + static_cast<int>(rng.bounded(75u))));
            for (int ox : {-size, 0, size})
                for (int oy : {-size, 0, size})
                    p.drawEllipse(dx-r/2+ox, dy-r/2+oy, r, r);
        }
    }

    // 8. Small sand spot — 1 in 4 tiles, 4-8 warm sandy blobs
    if (rng.bounded(7u) == 0) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(size)));
        int n  = 4 + static_cast<int>(rng.bounded(5u));
        p.setPen(Qt::NoPen);
        for (int i = 0; i < n; i++) {
            int dx = cx + static_cast<int>(rng.bounded(22u)) - 11;
            int dy = cy + static_cast<int>(rng.bounded(16u)) - 8;
            int r  = 5 + static_cast<int>(rng.bounded(9u));
            int v  = static_cast<int>(rng.bounded(18u)) - 9;
            p.setBrush(QColor(qBound(0,198+v,255), qBound(0,168+v,255), qBound(0,90+v,255),
                              100 + static_cast<int>(rng.bounded(70u))));
            for (int ox : {-size, 0, size})
                for (int oy : {-size, 0, size})
                    p.drawEllipse(dx-r/2+ox, dy-r/2+oy, r, r);
        }
    }

    p.end();
    return tile;
}

// Generates a dirt patch with organic edges and smooth gradient blending into grass (ARGB).
//
// Core technique: per-pixel alpha driven by bilinear-interpolated value noise.
// The noise offsets the distance-from-center threshold by ±30%, so the boundary
// is always irregular — never a circle or ellipse — and transitions gradually.
//   alpha = f(dist + noise * 0.55 - 0.28)²  →  0 at edges, ~195 at center
//
// On top: soil texture spots, stones with shadow/highlight, grass blades.
QPixmap ToolFunctions::generateDirtPatch(int w, int h, quint32 seed)
{
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QRandomGenerator rng(seed);

    // --- Coarse noise grid (9×9) for boundary distortion ---
    const int G = 9;
    float grid[G + 1][G + 1];
    for (int ny = 0; ny <= G; ny++)
        for (int nx = 0; nx <= G; nx++)
            grid[ny][nx] = rng.bounded(1000u) / 1000.0f;

    // Bilinear sample of the noise grid, u/v in [0,1]
    auto sampleNoise = [&](float u, float v) -> float {
        float fx = qBound(0.0f, u * G, static_cast<float>(G - 1));
        float fy = qBound(0.0f, v * G, static_cast<float>(G - 1));
        int ix = static_cast<int>(fx);
        int iy = static_cast<int>(fy);
        float tx = fx - ix, ty = fy - iy;
        return grid[iy  ][ix  ] * (1-tx) * (1-ty)
             + grid[iy  ][ix+1] *    tx  * (1-ty)
             + grid[iy+1][ix  ] * (1-tx) *    ty
             + grid[iy+1][ix+1] *    tx  *    ty;
    };

    // --- Per-pixel pass: soil color + noise-shaped alpha ---
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx   = 2.0f * (x - w * 0.5f) / w;
            float dy   = 2.0f * (y - h * 0.5f) / h;
            float dist = qSqrt(dx * dx + dy * dy);

            float n      = sampleNoise(static_cast<float>(x) / w, static_cast<float>(y) / h);
            // Noise shifts the boundary: some zones extend outward, others recede
            float factor = 1.0f - qBound(0.0f, dist + n * 0.55f - 0.28f, 1.0f);
            factor       = factor * factor; // quadratic: hard center, gentle fade

            if (factor < 0.015f) continue;

            int alpha = static_cast<int>(195.0f * factor);
            int r     = 54 + static_cast<int>(n * 14.0f);
            int g     = 30 + static_cast<int>(n *  9.0f);
            int b     =  7 + static_cast<int>(n *  7.0f);
            img.setPixel(x, y, qRgba(r, g, b, alpha));
        }
    }

    // --- Detail layers drawn on top with QPainter ---
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    // Soil texture spots (dark craters + lighter ridges)
    for (int i = 0; i < 22; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int r  = 4 + static_cast<int>(rng.bounded(16u));
        if (rng.bounded(2u) == 0)
            p.setBrush(QColor(28, 13,  2, 52 + static_cast<int>(rng.bounded(48u))));
        else
            p.setBrush(QColor(84, 53, 18, 35 + static_cast<int>(rng.bounded(38u))));
        p.drawEllipse(cx - r / 2, cy - r / 2, r, r);
    }

    // Stones: shadow + body + top highlight
    for (int i = 0; i < 20; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int rw = 3 + static_cast<int>(rng.bounded(9u));
        int rh = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw)));
        int a  = 162 + static_cast<int>(rng.bounded(68u));
        p.setBrush(QColor(42, 35, 26, a));
        p.drawEllipse(cx - rw / 2,     cy - rh / 2 + 1, rw,          rh);      // shadow
        p.setBrush(QColor(80, 68, 55, a));
        p.drawEllipse(cx - rw / 2,     cy - rh / 2,     rw,          rh);      // body
        p.setBrush(QColor(124, 110, 90, a / 2));
        p.drawEllipse(cx - rw / 3,     cy - rh / 2,     rw * 2 / 3,  rh / 2); // highlight
    }

    // Grass blades poking through
    const int blades = (w * h) / 520;
    for (int i = 0; i < blades; i++) {
        int x   = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int y   = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int len = 4 + static_cast<int>(rng.bounded(5u));
        int tlt = static_cast<int>(rng.bounded(5u)) - 2;
        QColor blade(
            32 + static_cast<int>(rng.bounded(22u)),
            75 + static_cast<int>(rng.bounded(38u)),
            10 + static_cast<int>(rng.bounded(14u)),
            145 + static_cast<int>(rng.bounded(65u))
        );
        p.setPen(QPen(blade, 1));
        p.drawLine(x, y, x + tlt, y - len);
    }

    p.end();
    return QPixmap::fromImage(img);
}

// ─── Vegetation chunk helper ────────────────────────────────────────────────
// All three vegetation chunks share the same per-pixel noise base technique.
// A local macro builds the noise grid and sampler to avoid repetition.
// ────────────────────────────────────────────────────────────────────────────

// Generates a forest floor patch (ARGB).
// Dark soil base, fallen leaves (autumn colors), twigs, moss tufts, sparse rocks.
QPixmap ToolFunctions::generateForestFloor(int w, int h, quint32 seed)
{
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QRandomGenerator rng(seed);

    const int G = 9;
    float grid[G+1][G+1];
    for (int ny = 0; ny <= G; ny++)
        for (int nx = 0; nx <= G; nx++)
            grid[ny][nx] = rng.bounded(1000u) / 1000.0f;
    auto sampleNoise = [&](float u, float v) -> float {
        float fx = qBound(0.0f, u*G, static_cast<float>(G-1));
        float fy = qBound(0.0f, v*G, static_cast<float>(G-1));
        int ix = static_cast<int>(fx), iy = static_cast<int>(fy);
        float tx = fx-ix, ty = fy-iy;
        return grid[iy][ix]*(1-tx)*(1-ty) + grid[iy][ix+1]*tx*(1-ty)
             + grid[iy+1][ix]*(1-tx)*ty   + grid[iy+1][ix+1]*tx*ty;
    };

    // --- Per-pixel base: very dark brown-green forest soil ---
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx = 2.0f*(x - w*0.5f)/w, dy = 2.0f*(y - h*0.5f)/h;
            float dist = qSqrt(dx*dx + dy*dy);
            float n = sampleNoise(static_cast<float>(x)/w, static_cast<float>(y)/h);
            float factor = 1.0f - qBound(0.0f, dist + n*0.55f - 0.28f, 1.0f);
            factor = factor * factor;
            if (factor < 0.015f) continue;
            img.setPixel(x, y, qRgba(
                42 + static_cast<int>(n*12.0f),
                35 + static_cast<int>(n*10.0f),
                14 + static_cast<int>(n* 7.0f),
                static_cast<int>(200.0f * factor)));
        }
    }

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    // Fallen leaves (autumn palette: browns, oranges, yellows, dark reds)
    const QColor leafColors[] = {
        QColor(148, 85, 28), QColor(188,112, 22), QColor(172,142, 32),
        QColor(118, 42, 18), QColor(158, 95, 20), QColor(135,118, 28),
    };
    for (int i = 0; i < 38; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int rw = 4 + static_cast<int>(rng.bounded(12u));
        int rh = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw/2)));
        int a  = 120 + static_cast<int>(rng.bounded(80u));
        QColor c = leafColors[rng.bounded(6u)];
        p.setBrush(QColor(c.red(), c.green(), c.blue(), a));
        p.drawEllipse(cx - rw/2, cy - rh/2, rw, rh);
    }

    // Twigs: short dark brown lines
    p.setPen(QPen(QColor(48, 28, 10, 180), 1, Qt::SolidLine, Qt::RoundCap));
    for (int i = 0; i < 18; i++) {
        int x1 = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int y1 = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int len = 8 + static_cast<int>(rng.bounded(18u));
        double angle = rng.generateDouble() * M_PI;
        int x2 = x1 + static_cast<int>(len * qCos(angle));
        int y2 = y1 + static_cast<int>(len * qSin(angle));
        p.drawLine(x1, y1, x2, y2);
    }
    p.setPen(Qt::NoPen);

    // Moss tufts: clusters of dark green dots
    for (int i = 0; i < 22; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int r  = 3 + static_cast<int>(rng.bounded(8u));
        int var = static_cast<int>(rng.bounded(20u)) - 10;
        p.setBrush(QColor(28+var, 72+var, 18+var, 130 + static_cast<int>(rng.bounded(80u))));
        p.drawEllipse(cx - r, cy - r, r*2, r*2);
    }

    // Sparse rocks (same 3-layer technique)
    for (int i = 0; i < 10; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int rw = 3 + static_cast<int>(rng.bounded(8u));
        int rh = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw)));
        int a  = 160 + static_cast<int>(rng.bounded(65u));
        p.setBrush(QColor(52, 44, 34, a));
        p.drawEllipse(cx-rw/2, cy-rh/2+1, rw, rh);
        p.setBrush(QColor(88, 76, 62, a));
        p.drawEllipse(cx-rw/2, cy-rh/2, rw, rh);
        p.setBrush(QColor(130, 115, 95, a/2));
        p.drawEllipse(cx-rw/3, cy-rh/2, rw*2/3, rh/2);
    }

    p.end();
    return QPixmap::fromImage(img);
}

// Generates a mossy rocks patch (ARGB).
// Grey-green base, large rocks with green moss overlay, vegetation between stones.
QPixmap ToolFunctions::generateMossyRocks(int w, int h, quint32 seed)
{
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QRandomGenerator rng(seed);

    const int G = 9;
    float grid[G+1][G+1];
    for (int ny = 0; ny <= G; ny++)
        for (int nx = 0; nx <= G; nx++)
            grid[ny][nx] = rng.bounded(1000u) / 1000.0f;
    auto sampleNoise = [&](float u, float v) -> float {
        float fx = qBound(0.0f, u*G, static_cast<float>(G-1));
        float fy = qBound(0.0f, v*G, static_cast<float>(G-1));
        int ix = static_cast<int>(fx), iy = static_cast<int>(fy);
        float tx = fx-ix, ty = fy-iy;
        return grid[iy][ix]*(1-tx)*(1-ty) + grid[iy][ix+1]*tx*(1-ty)
             + grid[iy+1][ix]*(1-tx)*ty   + grid[iy+1][ix+1]*tx*ty;
    };

    // --- Per-pixel base: dark grey-green (stone covered with moss) ---
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx = 2.0f*(x - w*0.5f)/w, dy = 2.0f*(y - h*0.5f)/h;
            float dist = qSqrt(dx*dx + dy*dy);
            float n = sampleNoise(static_cast<float>(x)/w, static_cast<float>(y)/h);
            float factor = 1.0f - qBound(0.0f, dist + n*0.55f - 0.28f, 1.0f);
            factor = factor * factor;
            if (factor < 0.015f) continue;
            img.setPixel(x, y, qRgba(
                62 + static_cast<int>(n*14.0f),
                72 + static_cast<int>(n*13.0f),
                48 + static_cast<int>(n*10.0f),
                static_cast<int>(195.0f * factor)));
        }
    }

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    // Large rocks: shadow + grey body + moss green overlay on top half
    for (int i = 0; i < 14; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int rw = 8 + static_cast<int>(rng.bounded(22u));
        int rh = 5 + static_cast<int>(rng.bounded(static_cast<quint32>(rw*2/3)));
        int a  = 170 + static_cast<int>(rng.bounded(60u));
        // Shadow
        p.setBrush(QColor(38, 34, 28, a));
        p.drawEllipse(cx-rw/2, cy-rh/2+2, rw, rh);
        // Stone body
        p.setBrush(QColor(88, 84, 76, a));
        p.drawEllipse(cx-rw/2, cy-rh/2, rw, rh);
        // Highlight
        p.setBrush(QColor(128, 122, 112, a/2));
        p.drawEllipse(cx-rw/3, cy-rh/2, rw*2/3, rh/2);
        // Moss overlay (green tint on upper part of rock)
        int mv = static_cast<int>(rng.bounded(20u)) - 10;
        p.setBrush(QColor(38+mv, 88+mv, 28+mv, 80 + static_cast<int>(rng.bounded(60u))));
        p.drawEllipse(cx-rw/2, cy-rh/2, rw, rh*2/3);
    }

    // Dense vegetation tufts between rocks
    const int vegCount = (w * h) / 200;
    for (int i = 0; i < vegCount; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        float dx = 2.0f*(cx-w*0.5f)/w, dy = 2.0f*(cy-h*0.5f)/h;
        float prob = 1.0f - qBound(0.0f, dx*dx+dy*dy, 1.0f);
        if (rng.generateDouble() > prob) continue;
        int r   = 1 + static_cast<int>(rng.bounded(5u));
        int var = static_cast<int>(rng.bounded(25u)) - 12;
        p.setBrush(QColor(25+var, 68+var, 15+var, 150 + static_cast<int>(rng.bounded(80u))));
        p.drawEllipse(cx-r, cy-r, r*2, r*2);
    }

    // Small pebbles scattered (sparse)
    const int pebbleCount = (w * h) / 500;
    for (int i = 0; i < pebbleCount; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        float dx = 2.0f*(cx-w*0.5f)/w, dy = 2.0f*(cy-h*0.5f)/h;
        float prob = 1.0f - qBound(0.0f, dx*dx+dy*dy, 1.0f);
        if (rng.generateDouble() > prob) continue;
        int rw = 2 + static_cast<int>(rng.bounded(5u));
        int rh = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw)));
        int a  = 155 + static_cast<int>(rng.bounded(65u));
        p.setBrush(QColor(48, 44, 36, a));
        p.drawEllipse(cx-rw/2, cy-rh/2+1, rw, rh);
        p.setBrush(QColor(82, 78, 68, a));
        p.drawEllipse(cx-rw/2, cy-rh/2, rw, rh);
        p.setBrush(QColor(118, 112, 98, a/2));
        p.drawEllipse(cx-rw/3, cy-rh/2, rw*2/3, rh/2);
    }

    p.end();
    return QPixmap::fromImage(img);
}

// Generates a dense undergrowth patch (ARGB).
// Rich dark green base, dense plant clusters, bare soil spots, few rocks, light leaf highlights.
QPixmap ToolFunctions::generateUndergrowth(int w, int h, quint32 seed)
{
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QRandomGenerator rng(seed);

    const int G = 9;
    float grid[G+1][G+1];
    for (int ny = 0; ny <= G; ny++)
        for (int nx = 0; nx <= G; nx++)
            grid[ny][nx] = rng.bounded(1000u) / 1000.0f;
    auto sampleNoise = [&](float u, float v) -> float {
        float fx = qBound(0.0f, u*G, static_cast<float>(G-1));
        float fy = qBound(0.0f, v*G, static_cast<float>(G-1));
        int ix = static_cast<int>(fx), iy = static_cast<int>(fy);
        float tx = fx-ix, ty = fy-iy;
        return grid[iy][ix]*(1-tx)*(1-ty) + grid[iy][ix+1]*tx*(1-ty)
             + grid[iy+1][ix]*(1-tx)*ty   + grid[iy+1][ix+1]*tx*ty;
    };

    // --- Per-pixel base: rich dark green ---
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx = 2.0f*(x - w*0.5f)/w, dy = 2.0f*(y - h*0.5f)/h;
            float dist = qSqrt(dx*dx + dy*dy);
            float n = sampleNoise(static_cast<float>(x)/w, static_cast<float>(y)/h);
            float factor = 1.0f - qBound(0.0f, dist + n*0.55f - 0.28f, 1.0f);
            factor = factor * factor;
            if (factor < 0.015f) continue;
            img.setPixel(x, y, qRgba(
                30 + static_cast<int>(n*10.0f),
                65 + static_cast<int>(n*18.0f),
                18 + static_cast<int>(n* 8.0f),
                static_cast<int>(198.0f * factor)));
        }
    }

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    // Bare soil patches (dark brown, sparse)
    for (int i = 0; i < 14; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int r  = 5 + static_cast<int>(rng.bounded(18u));
        p.setBrush(QColor(58, 38, 14, 45 + static_cast<int>(rng.bounded(40u))));
        p.drawEllipse(cx-r/2, cy-r/2, r, r);
    }

    // Dense vegetation clusters: dark greens, mid greens, yellow-greens
    const QColor vegColors[] = {
        QColor(22, 58, 12),  QColor(32, 80, 18),  QColor(28, 70, 15),
        QColor(45, 95, 22),  QColor(38, 85, 20),  QColor(52,108, 28),
    };
    const int vegCount = (w * h) / 120;
    for (int i = 0; i < vegCount; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        float dx = 2.0f*(cx-w*0.5f)/w, dy = 2.0f*(cy-h*0.5f)/h;
        float prob = 1.0f - qBound(0.0f, dx*dx+dy*dy, 1.0f);
        if (rng.generateDouble() > prob) continue;
        int r   = 1 + static_cast<int>(rng.bounded(6u));
        QColor c = vegColors[rng.bounded(6u)];
        p.setBrush(QColor(c.red(), c.green(), c.blue(), 145 + static_cast<int>(rng.bounded(90u))));
        p.drawEllipse(cx-r, cy-r, r*2, r*2);
    }

    // Light leaf highlights (small bright-green dots, sunlight catching leaves)
    for (int i = 0; i < (w*h)/600; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        float dx = 2.0f*(cx-w*0.5f)/w, dy = 2.0f*(cy-h*0.5f)/h;
        if (rng.generateDouble() > 1.0f - qBound(0.0f, dx*dx+dy*dy, 1.0f)) continue;
        int r = 1 + static_cast<int>(rng.bounded(3u));
        p.setBrush(QColor(88, 155, 45, 100 + static_cast<int>(rng.bounded(80u))));
        p.drawEllipse(cx-r, cy-r, r*2, r*2);
    }

    // Dead wood pieces: short thick dark lines
    p.setPen(QPen(QColor(52, 32, 12, 160), 2, Qt::SolidLine, Qt::RoundCap));
    for (int i = 0; i < 8; i++) {
        int x1 = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int y1 = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int len = 10 + static_cast<int>(rng.bounded(22u));
        double angle = rng.generateDouble() * M_PI;
        p.drawLine(x1, y1,
                   x1 + static_cast<int>(len*qCos(angle)),
                   y1 + static_cast<int>(len*qSin(angle)));
    }
    p.setPen(Qt::NoPen);

    // Few rocks (same 3-layer, very sparse)
    for (int i = 0; i < 6; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int rw = 3 + static_cast<int>(rng.bounded(9u));
        int rh = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw)));
        int a  = 162 + static_cast<int>(rng.bounded(65u));
        p.setBrush(QColor(50, 44, 34, a));
        p.drawEllipse(cx-rw/2, cy-rh/2+1, rw, rh);
        p.setBrush(QColor(84, 76, 62, a));
        p.drawEllipse(cx-rw/2, cy-rh/2, rw, rh);
        p.setBrush(QColor(122, 110, 92, a/2));
        p.drawEllipse(cx-rw/3, cy-rh/2, rw*2/3, rh/2);
    }

    p.end();
    return QPixmap::fromImage(img);
}

// Generates a flower field patch (ARGB). Picks 2–5 flower types randomly.
// Same per-pixel noise base as dirt/gravel (very subtle green tint, organic boundary).
// Flowers drawn like pebbles: shadow + body + highlight per petal, density falloff from center.
QPixmap ToolFunctions::generateFlowerField(int w, int h, quint32 seed)
{
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QRandomGenerator rng(seed);

    // --- Noise grid for organic boundary ---
    const int G = 9;
    float grid[G + 1][G + 1];
    for (int ny = 0; ny <= G; ny++)
        for (int nx = 0; nx <= G; nx++)
            grid[ny][nx] = rng.bounded(1000u) / 1000.0f;

    auto sampleNoise = [&](float u, float v) -> float {
        float fx = qBound(0.0f, u * G, static_cast<float>(G - 1));
        float fy = qBound(0.0f, v * G, static_cast<float>(G - 1));
        int ix = static_cast<int>(fx), iy = static_cast<int>(fy);
        float tx = fx - ix, ty = fy - iy;
        return grid[iy  ][ix  ] * (1-tx) * (1-ty)
             + grid[iy  ][ix+1] *    tx  * (1-ty)
             + grid[iy+1][ix  ] * (1-tx) *    ty
             + grid[iy+1][ix+1] *    tx  *    ty;
    };

    // --- Per-pixel pass: very subtle green tint to soften the grass transition ---
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx   = 2.0f * (x - w * 0.5f) / w;
            float dy   = 2.0f * (y - h * 0.5f) / h;
            float dist = qSqrt(dx * dx + dy * dy);
            float n    = sampleNoise(static_cast<float>(x) / w, static_cast<float>(y) / h);
            float factor = 1.0f - qBound(0.0f, dist + n * 0.55f - 0.28f, 1.0f);
            factor = factor * factor;
            if (factor < 0.015f) continue;
            img.setPixel(x, y, qRgba(52, 102, 28, static_cast<int>(38.0f * factor)));
        }
    }

    // --- Flower type definitions: shadow / body / highlight ---
    struct FlowerType { QColor shadow; QColor body; QColor highlight; };
    const FlowerType pool[] = {
        { QColor(185, 148,   0), QColor(255, 215,  25), QColor(250, 250, 150) }, // yellow
        { QColor(165, 165, 160), QColor(242, 242, 235), QColor(255, 255, 255) }, // white
        { QColor(108,  38, 148), QColor(172,  72, 212), QColor(218, 152, 248) }, // purple
        { QColor(140,  15,  15), QColor(215,  32,  32), QColor(250, 120,  95) }, // red
        { QColor( 38,  68, 172), QColor( 68, 110, 228), QColor(148, 182, 255) }, // blue
    };

    const int numTypes = 2 + static_cast<int>(rng.bounded(4u));
    QVector<int> used;
    while (used.size() < numTypes) {
        int t = static_cast<int>(rng.bounded(5u));
        if (!used.contains(t)) used.append(t);
    }

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    // Helper: draw one flower dot (shadow + body + highlight) with slight color variation
    auto drawFlower = [&](int cx, int cy, const FlowerType& ft) {
        int rw  = 2 + static_cast<int>(rng.bounded(5u));
        int rh  = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw)));
        int a   = 180 + static_cast<int>(rng.bounded(60u));
        // Per-flower color variation ±12 on each channel
        int var = static_cast<int>(rng.bounded(25u)) - 12;
        QColor body(
            qBound(0, ft.body.red()   + var, 255),
            qBound(0, ft.body.green() + var, 255),
            qBound(0, ft.body.blue()  + var, 255)
        );
        p.setBrush(ft.shadow);
        p.drawEllipse(cx - rw / 2, cy - rh / 2 + 1, rw,         rh);
        p.setBrush(body);
        p.drawEllipse(cx - rw / 2, cy - rh / 2,     rw,         rh);
        p.setBrush(QColor(ft.highlight.red(), ft.highlight.green(), ft.highlight.blue(), a / 2));
        p.drawEllipse(cx - rw / 3, cy - rh / 2,     rw * 2 / 3, rh / 2);
    };

    // --- Foliage layer: dark green dots scattered throughout (leaves/stems from above) ---
    // Drawn first so flowers appear on top
    const int foliageCount = (w * h) / 600;
    for (int i = 0; i < foliageCount; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));

        float dx   = 2.0f * (cx - w * 0.5f) / w;
        float dy   = 2.0f * (cy - h * 0.5f) / h;
        float prob = 1.0f - qBound(0.0f, (dx*dx + dy*dy), 1.0f);
        if (rng.generateDouble() > prob) continue;

        int r = 1 + static_cast<int>(rng.bounded(4u));
        int var = static_cast<int>(rng.bounded(20u)) - 10;
        p.setBrush(QColor(28 + var, 72 + var, 18 + var, 140 + static_cast<int>(rng.bounded(80u))));
        p.drawEllipse(cx - r, cy - r, r * 2, r * 2);
    }

    // --- Flower clusters: each cluster = one type, flowers placed within a small radius ---
    // More natural than uniform random: plants grow in groups
    const int clusterCount = (w * h) / 8000;
    for (int c = 0; c < clusterCount; c++) {
        // Cluster center with density falloff
        int ox = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int oy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        float dx   = 2.0f * (ox - w * 0.5f) / w;
        float dy   = 2.0f * (oy - h * 0.5f) / h;
        float prob = 1.0f - qBound(0.0f, dx*dx + dy*dy, 1.0f);
        if (rng.generateDouble() > prob) continue;

        // All flowers in a cluster share the same type
        const FlowerType& ft = pool[used[rng.bounded(static_cast<quint32>(numTypes))]];
        int clusterSize   = 4 + static_cast<int>(rng.bounded(10u));
        int clusterRadius = 12 + static_cast<int>(rng.bounded(20u));

        for (int f = 0; f < clusterSize; f++) {
            // Random offset within the cluster radius
            double angle = rng.generateDouble() * 2.0 * M_PI;
            double r     = rng.generateDouble() * clusterRadius;
            int fx = ox + static_cast<int>(r * qCos(angle));
            int fy = oy + static_cast<int>(r * qSin(angle));
            if (fx < 0 || fx >= w || fy < 0 || fy >= h) continue;
            drawFlower(fx, fy, ft);
        }
    }

    // --- Sparse individual flowers between clusters (fill gaps naturally) ---
    const int soloCount = (w * h) / 1800;
    for (int i = 0; i < soloCount; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        float dx   = 2.0f * (cx - w * 0.5f) / w;
        float dy   = 2.0f * (cy - h * 0.5f) / h;
        float prob = 1.0f - qBound(0.0f, dx*dx + dy*dy, 1.0f);
        if (rng.generateDouble() > prob) continue;
        drawFlower(cx, cy, pool[used[rng.bounded(static_cast<quint32>(numTypes))]]);
    }

    p.end();
    return QPixmap::fromImage(img);
}

// Generates a gravel patch with organic edges and smooth gradient blending into grass (ARGB).
// Generates a sand patch with organic edges (ARGB).
// Same per-pixel noise technique as generateDirtPatch, with warm sandy tones.
// Details: sandy texture spots + sparse embedded pebbles with density falloff.
QPixmap ToolFunctions::generateSandPatch(int w, int h, quint32 seed)
{
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QRandomGenerator rng(seed);

    // --- Coarse noise grid (9×9) ---
    const int G = 9;
    float grid[G + 1][G + 1];
    for (int ny = 0; ny <= G; ny++)
        for (int nx = 0; nx <= G; nx++)
            grid[ny][nx] = rng.bounded(1000u) / 1000.0f;

    auto sampleNoise = [&](float u, float v) -> float {
        float fx = qBound(0.0f, u * G, static_cast<float>(G - 1));
        float fy = qBound(0.0f, v * G, static_cast<float>(G - 1));
        int ix = static_cast<int>(fx), iy = static_cast<int>(fy);
        float tx = fx - ix, ty = fy - iy;
        return grid[iy  ][ix  ] * (1-tx) * (1-ty)
             + grid[iy  ][ix+1] *    tx  * (1-ty)
             + grid[iy+1][ix  ] * (1-tx) *    ty
             + grid[iy+1][ix+1] *    tx  *    ty;
    };

    // --- Per-pixel pass: warm sandy base + noise-shaped alpha ---
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx   = 2.0f * (x - w * 0.5f) / w;
            float dy   = 2.0f * (y - h * 0.5f) / h;
            float dist = qSqrt(dx * dx + dy * dy);
            float n      = sampleNoise(static_cast<float>(x) / w, static_cast<float>(y) / h);
            float factor = 1.0f - qBound(0.0f, dist + n * 0.55f - 0.28f, 1.0f);
            factor       = factor * factor;
            if (factor < 0.015f) continue;
            int alpha = static_cast<int>(200.0f * factor);
            // Sandy warm tones: noise adds subtle warm/cool variation
            int r = 195 + static_cast<int>(n * 20.0f);
            int g = 168 + static_cast<int>(n * 16.0f);
            int b =  95 + static_cast<int>(n * 14.0f);
            img.setPixel(x, y, qRgba(r, g, b, alpha));
        }
    }

    // --- Detail layers ---
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    // Sandy texture spots (dark hollows + light dunes)
    for (int i = 0; i < 24; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int r  = 6 + static_cast<int>(rng.bounded(22u));
        if (rng.bounded(2u) == 0)
            p.setBrush(QColor(155, 130, 65,  48 + static_cast<int>(rng.bounded(45u)))); // dark
        else
            p.setBrush(QColor(225, 205, 140, 38 + static_cast<int>(rng.bounded(38u)))); // light
        p.drawEllipse(cx - r / 2, cy - r / 2, r, r);
    }

    // Sparse embedded pebbles with density falloff (fewer than gravel)
    const int pebbleCount = (w * h) / 480;
    for (int i = 0; i < pebbleCount; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        float dx   = 2.0f * (cx - w * 0.5f) / w;
        float dy   = 2.0f * (cy - h * 0.5f) / h;
        float prob = 1.0f - qBound(0.0f, (dx*dx + dy*dy), 1.0f);
        if (rng.generateDouble() > prob) continue;
        int rw  = 2 + static_cast<int>(rng.bounded(7u));
        int rh  = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw)));
        int a   = 155 + static_cast<int>(rng.bounded(65u));
        int tone = static_cast<int>(rng.bounded(30u)) - 15;
        p.setBrush(QColor(145 + tone, 122 + tone,  78 + tone, a));      // shadow
        p.drawEllipse(cx - rw / 2,    cy - rh / 2 + 1, rw,         rh);
        p.setBrush(QColor(188 + tone, 162 + tone, 108 + tone, a));      // body
        p.drawEllipse(cx - rw / 2,    cy - rh / 2,     rw,         rh);
        p.setBrush(QColor(220 + tone, 200 + tone, 148 + tone, a / 2)); // highlight
        p.drawEllipse(cx - rw / 3,    cy - rh / 2,     rw * 2 / 3, rh / 2);
    }

    p.end();
    return QPixmap::fromImage(img);
}

// Same per-pixel noise technique as generateDirtPatch, with grey-beige soil tones
// and dense pebbles (shadow + body + highlight) as detail layer.
QPixmap ToolFunctions::generateGravelPatch(int w, int h, quint32 seed)
{
    QImage img(w, h, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    QRandomGenerator rng(seed);

    // --- Coarse noise grid (9×9) for boundary distortion ---
    const int G = 9;
    float grid[G + 1][G + 1];
    for (int ny = 0; ny <= G; ny++)
        for (int nx = 0; nx <= G; nx++)
            grid[ny][nx] = rng.bounded(1000u) / 1000.0f;

    auto sampleNoise = [&](float u, float v) -> float {
        float fx = qBound(0.0f, u * G, static_cast<float>(G - 1));
        float fy = qBound(0.0f, v * G, static_cast<float>(G - 1));
        int ix = static_cast<int>(fx);
        int iy = static_cast<int>(fy);
        float tx = fx - ix, ty = fy - iy;
        return grid[iy  ][ix  ] * (1-tx) * (1-ty)
             + grid[iy  ][ix+1] *    tx  * (1-ty)
             + grid[iy+1][ix  ] * (1-tx) *    ty
             + grid[iy+1][ix+1] *    tx  *    ty;
    };

    // --- Per-pixel pass: grey-beige base + noise-shaped alpha ---
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx   = 2.0f * (x - w * 0.5f) / w;
            float dy   = 2.0f * (y - h * 0.5f) / h;
            float dist = qSqrt(dx * dx + dy * dy);

            float n      = sampleNoise(static_cast<float>(x) / w, static_cast<float>(y) / h);
            float factor = 1.0f - qBound(0.0f, dist + n * 0.55f - 0.28f, 1.0f);
            factor       = factor * factor;

            if (factor < 0.015f) continue;

            int alpha = static_cast<int>(190.0f * factor);
            // Grey-beige base, noise adds subtle warm/cool variation
            int r = 145 + static_cast<int>(n * 18.0f);
            int g = 136 + static_cast<int>(n * 14.0f);
            int b = 115 + static_cast<int>(n * 12.0f);
            img.setPixel(x, y, qRgba(r, g, b, alpha));
        }
    }

    // --- Detail layers ---
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    // Sand/dust variation spots
    for (int i = 0; i < 20; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));
        int r  = 5 + static_cast<int>(rng.bounded(18u));
        if (rng.bounded(2u) == 0)
            p.setBrush(QColor(110, 100, 82, 45 + static_cast<int>(rng.bounded(45u)))); // darker
        else
            p.setBrush(QColor(175, 165, 142, 35 + static_cast<int>(rng.bounded(38u)))); // lighter
        p.drawEllipse(cx - r / 2, cy - r / 2, r, r);
    }

    // Pebbles: shadow + body + highlight, density fades with distance from center
    const int pebbleCount = (w * h) / 280;
    for (int i = 0; i < pebbleCount; i++) {
        int cx = static_cast<int>(rng.bounded(static_cast<quint32>(w)));
        int cy = static_cast<int>(rng.bounded(static_cast<quint32>(h)));

        // Probability of placing this pebble decreases toward the edges
        float dx   = 2.0f * (cx - w * 0.5f) / w;
        float dy   = 2.0f * (cy - h * 0.5f) / h;
        float dist = qSqrt(dx * dx + dy * dy);
        float prob = 1.0f - qBound(0.0f, dist * dist, 1.0f); // quadratic falloff
        if (rng.generateDouble() > prob) continue;

        int rw   = 2 + static_cast<int>(rng.bounded(7u));
        int rh   = 2 + static_cast<int>(rng.bounded(static_cast<quint32>(rw)));
        int a    = 155 + static_cast<int>(rng.bounded(70u));
        int tone = static_cast<int>(rng.bounded(40u)) - 20;
        p.setBrush(QColor(68 + tone, 60 + tone, 50 + tone, a));
        p.drawEllipse(cx - rw / 2,    cy - rh / 2 + 1, rw,         rh);      // shadow
        p.setBrush(QColor(118 + tone, 108 + tone, 92 + tone, a));
        p.drawEllipse(cx - rw / 2,    cy - rh / 2,     rw,         rh);      // body
        p.setBrush(QColor(168 + tone, 155 + tone, 135 + tone, a / 2));
        p.drawEllipse(cx - rw / 3,    cy - rh / 2,     rw * 2 / 3, rh / 2); // highlight
    }

    p.end();
    return QPixmap::fromImage(img);
}

#define IS_LEFT_SIDE(angle)  (angle < 270 && angle >= 90)
#define IS_RIGHT_SIDE(angle) (angle >= 270 || angle < 90)

QRectF ToolFunctions::getVisibleView(QGraphicsView * view)
{
    QPointF A = view->mapToScene( QPoint(0, 0) );
    QPointF B = view->mapToScene( QPoint(view->viewport()->width(), view->viewport()->height() ));
    return QRectF( A, B );
}

QRectF ToolFunctions::getBiggerView(QGraphicsView * view)
{
    QPointF A = view->mapToScene( QPoint(-300, -300) );
    QPointF B = view->mapToScene( QPoint(view->viewport()->width()+600, view->viewport()->height()+600 ));
    return QRectF( A, B );
}

int ToolFunctions::getAngleBetween(QGraphicsItem * instance1, QGraphicsItem * instance2)
{
    QPointF center1 = instance1->mapToScene(instance1->boundingRect().center());
    QPointF center2 = instance2->mapToScene(instance2->boundingRect().center());

    int angle = static_cast<int>(qRadiansToDegrees(qAtan2(
                        center1.y() - center2.y(),
                        center1.x() - center2.x()
                    )));

    if(angle < 0)   // Hero on top side
        angle += 360;

    return angle;
}

int ToolFunctions::getRandomAngle()
{
    int angle = 0;

    do{
        angle = QRandomGenerator::global()->bounded(360);
    }while(!ToolFunctions::isAllowedAngle(angle));

    return angle;
}

bool ToolFunctions::isOppositeDirection(int previous, int current)
{
    if( (IS_LEFT_SIDE(previous) && IS_RIGHT_SIDE(current))
     || (IS_LEFT_SIDE(current) && IS_RIGHT_SIDE(previous)))
    {
        return true;
    }
    return false;
}

int ToolFunctions::correct(int angle)
{
    if(angle > 90 && angle < 270)
    {
        // Correction when Y mirroring is applied
        return (180 - angle) - 180;
    }
    else
        return angle;
}

bool ToolFunctions::isAllowedAngle(int angle)
{
#define ANGLE_ALLOWED 100
    if((angle > (360 - ANGLE_ALLOWED/2) || angle < (ANGLE_ALLOWED/2))
    || (angle > (180 - ANGLE_ALLOWED/2) && angle < (180 + ANGLE_ALLOWED/2)))
    {
        return true;
    }
    else
        return false;
}

int ToolFunctions::getDistanceBeetween(QGraphicsItem * item1, QGraphicsItem * item2)
{
    QLineF line(item1->mapToScene(item1->boundingRect().width()/2, item1->boundingRect().height()/2),
                item2->mapToScene(item2->boundingRect().width()/2, item2->boundingRect().height()/2));
    return line.length();
}

int ToolFunctions::getNumberObstacles(QList<QGraphicsItem*> list)
{
    int count = 0;
    for(QGraphicsItem * item : qAsConst(list))
    {
        MapItem * mapItem = dynamic_cast<MapItem*>(item);
        if(mapItem){
            if(mapItem->isObstacle()){
                count++;
            }
        }
    }
    return count;
}

QRect ToolFunctions::getSpawnChunk(QSize size, QList<QGraphicsItem *> itemsToAvoid)
{
    QRect spawn;
    bool validatePosition = false;
    while(!validatePosition){
        validatePosition = true;
        for(QGraphicsItem * itemToAvoid : itemsToAvoid)
        {
            spawn = QRect(QPoint(QRandomGenerator::global()->bounded(500+size.width()/2, MAP_WIDTH-500-size.width()), QRandomGenerator::global()->bounded(size.height()/2+500, MAP_HEIGHT-500-size.height())), size);
            if(spawn.intersects(QRect(static_cast<int>(itemToAvoid->x()), static_cast<int>(itemToAvoid->y()), static_cast<int>(itemToAvoid->boundingRect().width()), static_cast<int>(itemToAvoid->boundingRect().height())))){
                validatePosition = false;
                break;
            }
        }
    }
    return spawn;
}
