// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/cell-grid.h>

using namespace Spreader;

auto CellGrid::getCell(Point coord) const noexcept -> Cell * {

    SPR_ASSERT_INPUT(coord.x < s_maxSize.width && coord.y < s_maxSize.height);
    
    if (!m_topTile) [[unlikely]]
        return nullptr;

    return m_topTile->get(coord);
} 




