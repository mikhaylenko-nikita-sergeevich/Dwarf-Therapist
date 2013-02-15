/*
Dwarf Therapist
Copyright (c) 2009 Trey Stout (chmod)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "dwarfdetailswidget.h"
#include "ui_dwarfdetailswidget.h"
#include "dwarftherapist.h"
#include "gamedatareader.h"
#include "dwarf.h"
#include "trait.h"
#include "dwarfstats.h"
#include "utils.h"
#include "dfinstance.h"
#include "fortressentity.h"
#include "caste.h"
#include "skill.h"
#include "attribute.h"
#include "sortabletableitems.h"

DwarfDetailsWidget::DwarfDetailsWidget(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , ui(new Ui::DwarfDetailsWidget)
{
    ui->setupUi(this);

    ui->splitter->setOpaqueResize(true);
    ui->splitter->setObjectName("details_splitter"); //important!! this name is used to find the splitter and save it's state!!

    int default_size = 60;

    ui->tw_skills->setColumnCount(4);
    ui->tw_skills->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_skills->setGridStyle(Qt::NoPen);
    ui->tw_skills->setAlternatingRowColors(true);
    ui->tw_skills->setHorizontalHeaderLabels(QStringList() << "Skill" << "Level" << "Bonus" << "Progress");
    ui->tw_skills->verticalHeader()->hide();
    ui->tw_skills->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
    ui->tw_skills->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
    ui->tw_skills->horizontalHeader()->setResizeMode(2, QHeaderView::Interactive);
    ui->tw_skills->horizontalHeader()->setStretchLastSection(true);
    ui->tw_skills->horizontalHeader()->resizeSection(0,100);
    ui->tw_skills->horizontalHeader()->resizeSection(1,default_size);
    ui->tw_skills->horizontalHeader()->resizeSection(2,default_size);

    ui->tw_attributes->setColumnCount(4);
    ui->tw_attributes->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_attributes->setWordWrap(true);
    ui->tw_attributes->setShowGrid(false);
    ui->tw_attributes->setGridStyle(Qt::NoPen);
    ui->tw_attributes->setAlternatingRowColors(true);
    ui->tw_attributes->setHorizontalHeaderLabels(QStringList() << "Attribute" << "Value" << "Max" << "Message");
    ui->tw_attributes->verticalHeader()->hide();
    ui->tw_attributes->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
    ui->tw_attributes->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
    ui->tw_attributes->horizontalHeader()->setResizeMode(2, QHeaderView::Interactive);
    ui->tw_attributes->horizontalHeader()->setStretchLastSection(true);
    ui->tw_attributes->horizontalHeader()->resizeSection(0,100);
    ui->tw_attributes->horizontalHeader()->resizeSection(1,default_size);
    ui->tw_attributes->horizontalHeader()->resizeSection(2,default_size);

    ui->tw_traits->setColumnCount(3);
    ui->tw_traits->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_traits->setWordWrap(true);
    ui->tw_traits->setShowGrid(false);
    ui->tw_traits->setGridStyle(Qt::NoPen);
    ui->tw_traits->setAlternatingRowColors(true);
    ui->tw_traits->setHorizontalHeaderLabels(QStringList() << "Trait" << "Raw" << "Message");
    ui->tw_traits->verticalHeader()->hide();
    ui->tw_traits->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
    ui->tw_traits->horizontalHeader()->setResizeMode(1, QHeaderView::Interactive);
    ui->tw_traits->horizontalHeader()->setStretchLastSection(true);
    ui->tw_traits->horizontalHeader()->resizeSection(0,100);
    ui->tw_traits->horizontalHeader()->resizeSection(1,default_size);

    ui->tw_roles->setColumnCount(2);
    ui->tw_roles->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_roles->setWordWrap(true);
    ui->tw_roles->setShowGrid(false);
    ui->tw_roles->setGridStyle(Qt::NoPen);
    ui->tw_roles->setAlternatingRowColors(true);
    ui->tw_roles->setHorizontalHeaderLabels(QStringList() << "Role" << "Rating");
    ui->tw_roles->verticalHeader()->hide();
    ui->tw_roles->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
    ui->tw_roles->horizontalHeader()->setStretchLastSection(true);
    ui->tw_roles->horizontalHeader()->resizeSection(0,100);

    ui->tw_prefs->setColumnCount(2);
    ui->tw_prefs->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tw_prefs->setWordWrap(true);
    ui->tw_prefs->setShowGrid(false);
    ui->tw_prefs->setGridStyle(Qt::NoPen);
    ui->tw_prefs->setAlternatingRowColors(true);
    ui->tw_prefs->setHorizontalHeaderLabels(QStringList() << "Type" << "Preferences");
    ui->tw_prefs->verticalHeader()->hide();
    ui->tw_prefs->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
    ui->tw_prefs->horizontalHeader()->setStretchLastSection(true);
    ui->tw_prefs->horizontalHeader()->resizeSection(0,100);

    //splitter
    m_splitter_sizes = DT->user_settings()->value("gui_options/detailPanesSizes").toByteArray();
    ui->splitter->restoreState(m_splitter_sizes);

    //TODO: store these as qpairs instead of individual vars
    //skill sorts
    m_skill_sort_col = 1;
    m_skill_sort_desc = Qt::DescendingOrder;
    ui->tw_skills->sortItems(m_skill_sort_col, static_cast<Qt::SortOrder>(m_skill_sort_desc));
    //attribute sorts
    m_attribute_sort_col = 0;
    m_attribute_sort_desc = Qt::AscendingOrder;
    ui->tw_attributes->sortItems(m_attribute_sort_col, static_cast<Qt::SortOrder>(m_attribute_sort_desc));
    //trait sorts
    m_trait_sort_col = 1;
    m_trait_sort_desc = Qt::DescendingOrder;
    ui->tw_traits->sortItems(m_trait_sort_col, static_cast<Qt::SortOrder>(m_trait_sort_desc));
    //role sorts
    m_role_sort_col = 1;
    m_role_sort_desc = Qt::DescendingOrder;
    ui->tw_roles->sortItems(m_role_sort_col, static_cast<Qt::SortOrder>(m_role_sort_desc));
    //pref sorts
    m_pref_sort_col = 0;
    m_pref_sort_desc = Qt::DescendingOrder;
    ui->tw_prefs->sortItems(m_pref_sort_col, static_cast<Qt::SortOrder>(m_pref_sort_desc));
}

void DwarfDetailsWidget::clear(){
    m_current_id = -1;
    //clear tables
    clear_table(*ui->tw_skills);
    clear_table(*ui->tw_attributes);
    clear_table(*ui->tw_traits);
    clear_table(*ui->tw_roles);
    clear_table(*ui->tw_prefs);
}

void DwarfDetailsWidget::show_dwarf(Dwarf *d) {
    if(d->id() == m_current_id)
        return;

    // Draw the name/profession text labels...
    ui->lbl_dwarf_name->setText(QString("<img src='%1'> %2").arg(d->gender_icon_path()).arg(d->nice_name()));
    ui->lbl_dwarf_name->setToolTip(tr("Name: %1").arg(ui->lbl_dwarf_name->text()));

    ui->lbl_age->setText(QString("Age: %1 years").arg(d->get_age()));
    ui->lbl_age->setToolTip(d->get_migration_desc());

    ui->lbl_translated_name->setText(QString("(%1)").arg(d->translated_name()));
    ui->lbl_translated_name->setToolTip(tr("Translated Name: %1").arg(ui->lbl_translated_name->text()));

    ui->lbl_profession->setText(QString("%1 %2").arg(embedPixmap(d->profession_icon())).arg(d->profession()));
    ui->lbl_profession->setToolTip(tr("Profession: %1").arg(ui->lbl_profession->text()));

    if(d->noble_position().isEmpty()){
        ui->lbl_noble_position->setText("");
        ui->lbl_noble_position->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Ignored);
        ui->lbl_noble->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Ignored);
    }
    else{
        ui->lbl_noble->setText(tr("<b>Noble Position%1</b>").arg(d->noble_position().contains(",") ? "s" : ""));
        ui->lbl_noble_position->setText(d->noble_position());
        ui->lbl_noble_position->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    }
    ui->lbl_noble_position->setToolTip(ui->lbl_noble_position->text());

    if(d->artifact_name().isEmpty()){
        ui->lbl_artifact->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        ui->lbl_artifact->setText("");
    }else{
        ui->lbl_artifact->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        ui->lbl_artifact->setText(tr("Creator of '%1'").arg(d->artifact_name()));
    }
    ui->lbl_artifact->setToolTip(ui->lbl_artifact->text());

    ui->lbl_current_job->setText(QString("%1").arg(d->current_job()));
    ui->lbl_current_job->setToolTip(tr("Job ID: %1").arg(QString::number(d->current_job_id())));

    GameDataReader *gdr = GameDataReader::ptr();

    Dwarf::DWARF_HAPPINESS happiness = d->get_happiness();
    ui->lbl_happiness->setText(QString("<b>%1</b> (%2)").arg(d->happiness_name(happiness)).arg(d->get_raw_happiness()));
    ui->lbl_happiness->setToolTip(d->get_thought_desc());
    QColor color = DT->user_settings()->value(
                QString("options/colors/happiness/%1").arg(static_cast<int>(happiness))).value<QColor>();
    QPalette p;
    QColor color2 = p.window().color();
    ui->lbl_happiness->setStyleSheet(QString("background: QLinearGradient(x1:0,y1:0,x2:0.9,y1:0,stop:0 %1, stop:1 %2); color: %3")
                                     .arg(color.name())
                                     .arg(color2.name())
                                     .arg(compliment(color).name())
                                     );

    if(DT->user_settings()->value("options/highlight_nobles",false).toBool() && d->noble_position() != ""){
        color = DT->get_DFInstance()->fortress()->get_noble_color(d->historical_id());
        ui->lbl_noble_position->setStyleSheet(QString("background: QLinearGradient(x1:0,y1:0,x2:0.9,y1:0,stop:0 %1, stop:1 %2); color: %3")
                                     .arg(color.name())
                                         .arg(color2.name())
                                         .arg(compliment(color).name())
                                         );
    }

    //clear tables
    clear_table(*ui->tw_skills);
    clear_table(*ui->tw_attributes);
    clear_table(*ui->tw_traits);
    clear_table(*ui->tw_roles);
    clear_table(*ui->tw_prefs);

    // SKILLS TABLE
    QVector<Skill> *skills = d->get_skills();
    ui->tw_skills->setSortingEnabled(false);
    int real_count = 0;
    int raw_bonus_xp = 100;
    int bonus_xp = 0;
    QString tooltip = "";
    for (int row = 0; row < skills->size(); ++row) {
        Skill s = skills->at(row);
        if(s.capped_level() > -1)
        {
            real_count = ui->tw_skills->rowCount();
            ui->tw_skills->insertRow(real_count);
            ui->tw_skills->setRowHeight(real_count, 18);

            QTableWidgetItem *item_skill = new QTableWidgetItem(s.name());
            tooltip = tr("<center><h4>%1</h4></center>").arg(s.name());
            if(s.id()==d->highest_moodable().id()){
                if(d->had_mood()){
                    item_skill->setBackgroundColor(QColor(153,102,34,255));
                    tooltip.append(tr("<p>Has already had a mood!</p>"));
                }
                else{
                    item_skill->setBackgroundColor(QColor(220, 220, 255, 255));
                    tooltip.append(tr("<p>This is the highest moodable skill.</p>"));
                }
            }
            item_skill->setToolTip(tooltip);

            sortableFloatTableWidgetItem *item_level = new sortableFloatTableWidgetItem();
            item_level->setText(QString::number(d->skill_level(s.id())));
            item_level->setData(Qt::UserRole, (float)d->skill_level(s.id(),true,true));
            item_level->setTextAlignment(Qt::AlignHCenter);
            if(s.rust_rating() != ""){
                item_level->setBackgroundColor(s.rust_color());
                item_level->setForeground(Qt::black);
            }
            item_level->setToolTip(s.to_string(true,true,false));


            raw_bonus_xp = s.skill_rate();
            bonus_xp = raw_bonus_xp - 100;
            sortableFloatTableWidgetItem *item_bonus = new sortableFloatTableWidgetItem();
            item_bonus->setText(QString::number(bonus_xp,'f',0)+"%");
            item_bonus->setData(Qt::UserRole, bonus_xp);
            item_bonus->setTextAlignment(Qt::AlignHCenter);
            if(bonus_xp != 0)
                item_bonus->setToolTip(tr("Receives %1% <b>%2</b> experience than normal. (RAW: %3%)")
                                       .arg(abs(bonus_xp))
                                       .arg(bonus_xp > 0 ? "more" : "less")
                                       .arg(raw_bonus_xp));
            if(bonus_xp < 0){
                item_bonus->setBackground(QColor(204, 0, 0, 128));
                item_bonus->setForeground(QColor(0, 0, 128, 255));
            }

            QProgressBar *pb = new QProgressBar(ui->tw_skills);
            pb->setRange(s.exp_for_current_level(), s.exp_for_next_level());
            if(s.is_losing_xp())
                pb->setValue(s.exp_for_next_level());
            else
                pb->setValue(s.actual_exp());
            pb->setDisabled(true);// this is to keep them from animating and looking all goofy
            pb->setToolTip(s.exp_summary());

            ui->tw_skills->setItem(real_count, 0, item_skill);
            ui->tw_skills->setItem(real_count, 1, item_level);
            ui->tw_skills->setItem(real_count, 2, item_bonus);
            ui->tw_skills->setCellWidget(real_count, 3, pb);
        }
    }        
    ui->tw_skills->setSortingEnabled(true);
    if(!DT->multiple_castes)
        ui->tw_skills->hideColumn(2);

    // ATTRIBUTES TABLE
    QVector<Attribute> *attributes = d->get_attributes();
    ui->tw_attributes->setSortingEnabled(false);
    for (int row = 0; row < attributes->size(); ++row) {
        ui->tw_attributes->insertRow(0);
        ui->tw_attributes->setRowHeight(0, 18);
        Attribute a = attributes->at(row);

        QTableWidgetItem *attribute_name = new QTableWidgetItem(a.get_name());
        tooltip = tr("<center><h4>%1</h4></center>").arg(a.get_name());
        attribute_name->setToolTip(tooltip);

        sortableFloatTableWidgetItem *attribute_rating = new sortableFloatTableWidgetItem;
        attribute_rating->setTextAlignment(Qt::AlignHCenter);
        attribute_rating->setText(QString::number(a.value()));
        attribute_rating->setData(Qt::UserRole, a.value());
        attribute_rating->setToolTip(a.get_value_display());
        if(a.get_descriptor_rank() <= 3) { //3 is the last bin before the median group
            attribute_rating->setBackground(QColor(204, 0, 0, 128));
            attribute_rating->setForeground(QColor(0, 0, 128, 255));
        }
        attribute_rating->setToolTip(a.get_value_display());

        sortableFloatTableWidgetItem *attribute_max = new sortableFloatTableWidgetItem;
        attribute_max->setTextAlignment(Qt::AlignHCenter);
        attribute_max->setText(QString::number(a.max()));
        attribute_max->setData(Qt::UserRole, a.max());        
        attribute_max->setToolTip(QString("%1 (%2)").arg((int)a.max()).arg(tr("This is the maximum attainable value.")));

        //don't show the 'average for a <caste>' in the details pane
        QString lvl_msg;
        if(a.get_descriptor_rank() == 4)
            lvl_msg = "";
        else
            lvl_msg = a.get_descriptor();

        QTableWidgetItem *attribute_msg = new QTableWidgetItem(lvl_msg);
        attribute_msg->setToolTip(lvl_msg);
        ui->tw_attributes->setItem(0, 0, attribute_name);
        ui->tw_attributes->setItem(0, 1, attribute_rating);
        ui->tw_attributes->setItem(0, 2, attribute_max);
        ui->tw_attributes->setItem(0, 3, attribute_msg);
    }
    ui->tw_attributes->setSortingEnabled(true);


    // TRAITS TABLE
    QHash<int, short> traits = d->traits();
    ui->tw_traits->setSortingEnabled(false);
    for (int row = 0; row < traits.size(); ++row) {
        short val = traits[row];

        if (d->trait_is_active(row))
        {
            ui->tw_traits->insertRow(0);
            ui->tw_traits->setRowHeight(0, 18);
            Trait *t = gdr->get_trait(row);
            QTableWidgetItem *trait_name = new QTableWidgetItem(t->name);
            trait_name->setToolTip(tr("<center><h4>%1</h4></center>").arg(t->name));
            QTableWidgetItem *trait_score = new QTableWidgetItem;
            trait_score->setTextAlignment(Qt::AlignHCenter);
            trait_score->setData(0, val);

            int deviation = abs(50 - val);
            if (deviation >= 41) {
                trait_score->setBackground(QColor(0, 0, 128, 255));
                trait_score->setForeground(QColor(255, 255, 255, 255));
            } else if (deviation >= 25) {
                trait_score->setBackground(QColor(220, 220, 255, 255));
                trait_score->setForeground(QColor(0, 0, 128, 255));
            }

            QTableWidgetItem *trait_msg = new QTableWidgetItem();
            QString msg = t->level_message(val);
            QString temp = t->conflicts_messages(val);
            if(!temp.isEmpty())
                msg.append(". " + temp);
            temp = t->special_messages(val);
            if(!temp.isEmpty())
                msg.append(". " + temp);
            trait_msg->setText(msg);

            trait_msg->setToolTip(QString("%1<br/>%2<br/>%3")
                                  .arg(t->level_message(val))
                                  .arg(t->conflicts_messages(val))
                                  .arg(t->special_messages(val)));

            ui->tw_traits->setItem(0, 0, trait_name);
            ui->tw_traits->setItem(0, 1, trait_score);
            ui->tw_traits->setItem(0, 2, trait_msg);
        }
    }
    ui->tw_traits->setSortingEnabled(true);

    // ROLES TABLE
    QList<QPair<QString, float> > roles = d->sorted_role_ratings();
    ui->tw_roles->setSortingEnabled(false);
    QString name = "";
    float val = 0.0;
    int max = DT->user_settings()->value("options/role_count_pane",10).toInt();
    if(max > d->sorted_role_ratings().count())
        max = d->sorted_role_ratings().count();
    for(int i = 0; i < max; i++){
        name = roles.at(i).first;
        val = roles.at(i).second;

        ui->tw_roles->insertRow(0);
        ui->tw_roles->setRowHeight(0, 18);

        QTableWidgetItem *role_name = new QTableWidgetItem(name);
        role_name->setToolTip(tr("<center><h4>%1</h4></center>").arg(name));
        sortableFloatTableWidgetItem *role_rating = new sortableFloatTableWidgetItem();
        role_rating->setText(QString::number(val,'f',2)+"%");
        role_rating->setData(Qt::UserRole,val);
        role_rating->setTextAlignment(Qt::AlignHCenter);

        if (val < 50) {
            role_rating->setBackground(QColor(204, 0, 0, 128));
            role_rating->setForeground(QColor(0, 0, 128, 255));
        }

        ui->tw_roles->setItem(0, 0, role_name);
        ui->tw_roles->setItem(0, 1, role_rating);

        Role *r = gdr->get_role(name);
        if(r)
            role_rating->setToolTip(r->get_role_details());
    }
    ui->tw_roles->setSortingEnabled(true);


    // PREFERENCES TABLE
    ui->tw_prefs->setSortingEnabled(false);

    QString prefs;
    foreach(QString name, d->get_grouped_preferences().uniqueKeys()){

        prefs = capitalize(d->get_grouped_preferences().value(name)->join(", "));

        ui->tw_prefs->insertRow(0);
        ui->tw_prefs->setRowHeight(0, 18);

        QTableWidgetItem *pref_type = new QTableWidgetItem(name);
        pref_type->setToolTip(tr("<center><h4>%1</h4></center>").arg(name));
        QTableWidgetItem *pref_values = new QTableWidgetItem(prefs);

        ui->tw_prefs->setItem(0, 0, pref_type);
        ui->tw_prefs->setItem(0, 1, pref_values);

        pref_values->setToolTip(prefs);
    }
    ui->tw_prefs->setSortingEnabled(true);

    m_current_id = d->id();
    d = 0;
}

void DwarfDetailsWidget::clear_table(QTableWidget &t){
    for(int i = t.rowCount(); i >=0; i--){
        t.removeRow(i);
    }
}


