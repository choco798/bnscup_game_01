# Grade Selection System Design

## Current System Analysis
- Problems are organized by grades (0-2) in `problems.json`
- `ProblemManager` has grade-specific problem arrays
- Player's current rank is tracked in `SaveData`
- Ranks progress from 特待生 -> 名人 -> 達人 -> 俳人

## Proposed Changes

### 1. Grade Selection UI
Add a grade selection interface to the title screen:
- Display available grades based on player's current rank
- Show grade selection buttons in a vertical layout
- Visual indication of locked/unlocked grades
- Grade buttons should display:
  - Grade name/level
  - Number of problems in that grade
  - Player's progress for that grade

### 2. ProblemManager Updates
```cpp
class ProblemManager {
    // Add new functionality
    bool isGradeAvailable(int32 grade, size_t playerRank);
    void setSelectedGrade(int32 grade);
    int32 getSelectedGrade() const;
    
private:
    int32 m_selectedGrade = -1; // Currently selected grade
};
```

### 3. GameState Updates
```cpp
class GameState {
    // Add new member
    int32 selectedGrade = -1;
    
    // Add new methods
    void setSelectedGrade(int32 grade);
    int32 getSelectedGrade() const;
    bool isGradeAvailable(int32 grade) const;
};
```

### 4. TitleScene Updates
```cpp
class TitleScene {
private:
    // Add grade selection buttons
    Array<ui::Button> m_gradeButtons;
    
    // Add methods
    void initializeGradeButtons();
    void updateGradeButtons();
    void drawGradeButtons() const;
};
```

## Grade Availability Rules
1. Player can always play their current grade
2. Player can play any grade lower than their current grade
3. Higher grades remain locked until promotion
4. Visual feedback shows locked/unlocked status

## UI Layout
```
[Title]
[Start Game] <- Remove or relocate
[Grade Selection] <- New section
  [Grade 0] <- Button with progress
  [Grade 1] <- Button with progress/lock
  [Grade 2] <- Button with progress/lock
[How to Play]
[Exit]
```

## Implementation Steps
1. Add grade selection UI components to TitleScene
2. Implement grade availability checking in GameState
3. Update ProblemManager to filter problems by selected grade
4. Add visual feedback for locked/unlocked grades
5. Save selected grade preference in GameState
6. Update problem loading to respect grade selection

## Testing Scenarios
1. New player (Rank 0)
   - Should only see Grade 0 unlocked
   - Cannot select higher grades

2. Mid-rank player
   - Can select current grade
   - Can select any lower grade
   - Higher grades remain locked

3. Progress tracking
   - Progress shown correctly for each grade
   - Grade completion status preserved

4. Problem distribution
   - Only problems from selected grade appear
   - Problem order randomized within grade