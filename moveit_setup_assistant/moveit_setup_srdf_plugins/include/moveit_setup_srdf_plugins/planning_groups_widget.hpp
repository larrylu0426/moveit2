/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Dave Coleman */

#pragma once

// Qt
#include <QPushButton>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

// SA
#include <moveit_setup_framework/qt/setup_step_widget.hpp>
#include <moveit_setup_framework/qt/double_list_widget.hpp>
#include <moveit_setup_srdf_plugins/planning_groups.hpp>
#include <moveit_setup_srdf_plugins/kinematic_chain_widget.hpp>  // for kinematic chain page
#include <moveit_setup_srdf_plugins/group_edit_widget.hpp>       // for group rename page

// Forward Declaration (outside of namespace for Qt)
class PlanGroupType;

namespace moveit_setup
{
namespace srdf_setup
{
// Custom Type
enum GroupType
{
  JOINT = 1,
  LINK = 2,
  CHAIN = 3,
  SUBGROUP = 4,
  GROUP = 5
};

// ******************************************************************************************
// ******************************************************************************************
// CLASS
// ******************************************************************************************
// ******************************************************************************************
class PlanningGroupsWidget : public SetupStepWidget
{
  Q_OBJECT

public:
  // ******************************************************************************************
  // Public Functions
  // ******************************************************************************************

  void onInit() override;

  void changeScreen(int index);

  /// Received when this widget is chosen from the navigation menu
  void focusGiven() override;

  SetupStep& getSetupStep() override
  {
    return setup_step_;
  }

private Q_SLOTS:

  // ******************************************************************************************
  // Slot Event Functions
  // ******************************************************************************************

  /// Displays data in the link_pairs_ data structure into a QtTableWidget
  void loadGroupsTree();

  /// Highlight the group of whatever element is selected in the tree view
  void previewSelected();

  /// Edit whatever element is selected in the tree view
  void editSelected();

  /// Create a new, empty group
  void addGroup();

  /// Call when screen is done being edited
  void saveJointsScreen();
  void saveLinksScreen();
  void saveChainScreen();
  void saveSubgroupsScreen();
  void saveGroupScreenEdit();
  void saveGroupScreenJoints();
  void saveGroupScreenLinks();
  void saveGroupScreenChain();
  void saveGroupScreenSubgroups();

  // Delete a group
  void deleteGroup();

  /// Call when edit screen is canceled
  void cancelEditing();

  /// Called when user clicks link part of bottom left label
  void alterTree(const QString& link);

  /// Called from Double List widget to highlight a link
  void previewSelectedLink(const std::vector<std::string>& links);

  /// Called from Double List widget to highlight a joint
  // void previewClickedJoint( std::string name );
  void previewSelectedJoints(const std::vector<std::string>& joints);

  /// Called from Double List widget to highlight a subgroup
  void previewSelectedSubgroup(const std::vector<std::string>& groups);

private:
  // ******************************************************************************************
  // Qt Components
  // ******************************************************************************************

  /// Main table for holding groups
  QTreeWidget* groups_tree_;

  /// For changing between table and different add/edit views
  QStackedWidget* stacked_widget_;

  /// Show and hide edit button
  QPushButton* btn_edit_;

  QPushButton* btn_delete_;

  // Stacked Layout SUBPAGES -------------------------------------------

  QWidget* groups_tree_widget_;
  DoubleListWidget* joints_widget_;
  DoubleListWidget* links_widget_;
  DoubleListWidget* subgroups_widget_;
  KinematicChainWidget* chain_widget_;
  GroupEditWidget* group_edit_widget_;

  // ******************************************************************************************
  // Variables
  // ******************************************************************************************

  PlanningGroups setup_step_;

  /// Remember what group we are editing when an edit screen is being shown
  std::string current_edit_group_;

  /// Remember to which editing screen we should return on Cancel
  int return_screen_;

  /// Remember whethere we're editing a group or adding a new one
  bool adding_new_group_;

  // ******************************************************************************************
  // Private Functions
  // ******************************************************************************************

  /// Builds the main screen list widget
  QWidget* createContentsWidget();

  /// Recursively build the SRDF tree
  void loadGroupsTreeRecursive(srdf::Model::Group& group_it, QTreeWidgetItem* parent);

  // Load edit screen
  void loadJointsScreen(srdf::Model::Group* this_group);
  void loadLinksScreen(srdf::Model::Group* this_group);
  void loadChainScreen(srdf::Model::Group* this_group);
  void loadSubgroupsScreen(srdf::Model::Group* this_group);
  void loadGroupScreen(srdf::Model::Group* this_group);

  // Save group screen
  bool saveGroupScreen();

  /// Switch to current groups view
  void showMainScreen();
};

// ******************************************************************************************
// ******************************************************************************************
// Metatype Class For Holding Points to Group Parts
// ******************************************************************************************
// ******************************************************************************************

class PlanGroupType
{
public:
  //  explicit PlanGroupType();
  PlanGroupType()
  {
  }
  PlanGroupType(srdf::Model::Group* group, const GroupType type);
  virtual ~PlanGroupType()
  {
    ;
  }

  srdf::Model::Group* group_;

  GroupType type_;
};

}  // namespace srdf_setup
}  // namespace moveit_setup

Q_DECLARE_METATYPE(moveit_setup::srdf_setup::PlanGroupType);
